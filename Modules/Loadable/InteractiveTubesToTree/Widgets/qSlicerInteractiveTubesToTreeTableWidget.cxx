/*=========================================================================

Library:   VesselView

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
//Qt imports
#include <QStandardItemModel>
#include <QSignalMapper>
#include <QMenuBar>
#include <QDebug>
//CTK imports
#include <ctkColorPickerButton.h>

// SlicerQt includes
#include "qSlicerApplication.h"

// Table Widgets includes
#include "qSlicerInteractiveTubesToTreeTableWidget.h"
#include "ui_qSlicerInteractiveTubesToTreeTableWidget.h"
#include "vtkSlicerInteractiveTubesToTreeLogic.h"

//vtk includes
#include <vtkDataSet.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>

// MRML includes
#include <vtkMRMLSpatialObjectsNode.h>
#include <vtkMRMLSpatialObjectsDisplayNode.h>
#include <vtkMRMLSpatialObjectsLineDisplayNode.h>
#include <vtkMRMLSpatialObjectsDisplayPropertiesNode.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLMarkupsNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_InteractiveTubesToTree
class qSlicerInteractiveTubesToTreeTableWidgetPrivate
  : public Ui_qSlicerInteractiveTubesToTreeTableWidget
{
  Q_DECLARE_PUBLIC(qSlicerInteractiveTubesToTreeTableWidget);
protected:
  qSlicerInteractiveTubesToTreeTableWidget* const q_ptr;

public:
  qSlicerInteractiveTubesToTreeTableWidgetPrivate(
    qSlicerInteractiveTubesToTreeTableWidget& object);
  vtkSlicerInteractiveTubesToTreeLogic* logic() const;
  virtual void setupUi(qSlicerInteractiveTubesToTreeTableWidget*);

  void init();
  vtkMRMLSpatialObjectsNode* SpatialObjectsNode;
  vtkMRMLSpatialObjectsDisplayNode* SpatialObjectsDisplayNode;
  vtkMRMLMarkupsNode* MarkupsNode;
  int columnIndex(QString label);
private:
  QStringList columnLabels;
};

// --------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidgetPrivate
::qSlicerInteractiveTubesToTreeTableWidgetPrivate(
  qSlicerInteractiveTubesToTreeTableWidget& object)
  : q_ptr(&object)
{
  this->SpatialObjectsNode = 0;
  this->SpatialObjectsDisplayNode = 0;
  this->MarkupsNode = 0;
  this->columnLabels << "Tube ID" << "Color" << "Is Root" << "Is Artery" << "Parent Id";
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidgetPrivate
::setupUi(qSlicerInteractiveTubesToTreeTableWidget* widget)
{
  this->Ui_qSlicerInteractiveTubesToTreeTableWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidgetPrivate::columnIndex(QString label)
{
  return this->columnLabels.indexOf(label);
}

//-----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic* qSlicerInteractiveTubesToTreeTableWidgetPrivate::logic() const
{
  Q_Q(const qSlicerInteractiveTubesToTreeTableWidget);
  return vtkSlicerInteractiveTubesToTreeLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeTableWidget methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidget
::qSlicerInteractiveTubesToTreeTableWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerInteractiveTubesToTreeTableWidgetPrivate(*this) )
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  d->init();

  // disable as there is not MRML Node associated with the widget
  this->setEnabled(false);
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidget
::~qSlicerInteractiveTubesToTreeTableWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidgetPrivate::init()
{
  Q_Q(qSlicerInteractiveTubesToTreeTableWidget);

  this->setupUi(q);

  this->SelectTubeColorPicker->setColor(QColor(0,0,255));
  this->SelectTubeColorPicker->setDisplayColorName(false);
  QObject::connect(this->SelectTubeColorPicker,
        SIGNAL(colorChanged(QColor)), q,
        SLOT(onSelectTubeColorChanged(QColor)));
  QIcon pushButtonIcon;
  pushButtonIcon.addFile(QString::fromUtf8(":AnnotationPointWithArrow.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectTubesPushButton->setIcon(pushButtonIcon);
  QObject::connect(
    this->SelectTubesPushButton, SIGNAL(toggled(bool)),
    q, SLOT(updateMRMLFromWidget()));

  this->ShowRootsColorPicker->setColor(QColor(255,255,0));
  this->ShowRootsColorPicker->setDisplayColorName(false);
  QObject::connect(this->ShowRootsColorPicker,
        SIGNAL(colorChanged(QColor)), q,
        SLOT(onShowRootsColorChanged(QColor)));
  pushButtonIcon.addFile(QString::fromUtf8(":ColorYellow1.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->ShowRootsPushButton->setIcon(pushButtonIcon);
  this->ShowRootsPushButton->setCheckable(true);
  QObject::connect(
    this->ShowRootsPushButton, SIGNAL(toggled(bool)),
    q, SLOT(onClickShowRoots(bool)));
  
  this->ShowOrphansColorPicker->setColor(QColor(0,255,0));
  this->ShowOrphansColorPicker->setDisplayColorName(false);
  QObject::connect(this->ShowOrphansColorPicker,
        SIGNAL(colorChanged(QColor)), q,
        SLOT(onShowOrphansColorChanged(QColor)));
  pushButtonIcon.addFile(QString::fromUtf8(":ColorGreen1.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->ShowOrphansPushButton->setIcon(pushButtonIcon);
  this->ShowOrphansPushButton->setCheckable(true);
  QObject::connect(
    this->ShowOrphansPushButton, SIGNAL(toggled(bool)),
    q, SLOT(onClickShowOrphans(bool)));

 // qSlicerApplication * app = qSlicerApplication::application();
 // vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
//  q->qvtkReconnect(interactionNode, vtkCommand::ModifiedEvent,
 //   q, SLOT(updateWidgetFromMRML()));

  this->TableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->TableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
  this->TableWidget->setColumnCount(this->columnLabels.size());
  this->TableWidget->setHorizontalHeaderLabels(this->columnLabels);
  this->TableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  
  // listen for click on a cell
  QObject::connect(this->TableWidget, SIGNAL(itemClicked(QTableWidgetItem*)),
    q, SLOT(onTableCellClicked(QTableWidgetItem*)));
  QObject::connect(this->TableWidget, SIGNAL(itemSelectionChanged()),
    q, SLOT(onTableSelectionChanged()));

  QObject::connect(this->TableWidget->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)),
    q, SLOT(onClickHorizontalHeader(int)));
  QObject::connect(this->TableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)),
    q, SLOT(onClickHorizontalHeader(int)));

  pushButtonIcon.addFile(QString::fromUtf8(":MarkSelected.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->ApplyColorPushButton->setIcon(pushButtonIcon);
//  QObject::connect(this->MarkSelectedAsRootPushButton, SIGNAL(clicked()),
//    q, SLOT(onClickMarkSelectedAsRoot()));

  pushButtonIcon.addFile(QString::fromUtf8(":DeleteSelected.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->DeleteSelectedPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->DeleteSelectedPushButton, SIGNAL(clicked()),
    q, SLOT(onClickDeleteSelected()));

  pushButtonIcon.addFile(QString::fromUtf8(":Select.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectAllRootsPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->SelectAllRootsPushButton, SIGNAL(clicked()),
    q, SLOT(onClickSelectAllRoots()));

  pushButtonIcon.addFile(QString::fromUtf8(":Select.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectAllOrphansPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->SelectAllOrphansPushButton, SIGNAL(clicked()),
    q, SLOT(onClickSelectAllOrphans()));

  pushButtonIcon.addFile(QString::fromUtf8(":Show.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->ShowHidePushButton->setIcon(pushButtonIcon);
  this->ShowHidePushButton->setCheckable(true);
  QObject::connect(this->ShowHidePushButton,
        SIGNAL(toggled(bool)),
        q, SLOT(onClickShowHideTubes(bool)));

  q->setEnabled(this->SpatialObjectsNode != 0);
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayNode* qSlicerInteractiveTubesToTreeTableWidget::
SpatialObjectsDisplayNode() const
{
  Q_D(const qSlicerInteractiveTubesToTreeTableWidget);

  return d->SpatialObjectsDisplayNode;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget
::setSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget
::setSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->SpatialObjectsNode == node)
  {
    return;
  }
  
  vtkMRMLSpatialObjectsNode *oldNode = d->SpatialObjectsNode;
  d->SpatialObjectsNode = node;
  this->setSpatialObjectsDisplayNode(
    d->SpatialObjectsNode ? d->SpatialObjectsNode->GetNthDisplayNode(1) : NULL);

  qvtkReconnect(oldNode, d->SpatialObjectsNode, vtkCommand::ModifiedEvent,
    this, SLOT(updateWidgetFromMRML()));

  //calling tube display node
  buildTubeDisplayTable();

//  qSlicerApplication * app = qSlicerApplication::application();
//  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
//  interactionNode->SetPlaceModePersistence(1);

  this->updateWidgetFromMRML();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::
setSpatialObjectsDisplayNode(vtkMRMLNode* node)
{
  this->setSpatialObjectsDisplayNode(
    vtkMRMLSpatialObjectsDisplayNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::
setSpatialObjectsDisplayNode(vtkMRMLSpatialObjectsDisplayNode*
SpatialObjectsDisplayNode)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->SpatialObjectsDisplayNode == SpatialObjectsDisplayNode)
  {
    return;
  }

  vtkMRMLSpatialObjectsDisplayNode *oldDisplayNode =
    this->SpatialObjectsDisplayNode();
  d->SpatialObjectsDisplayNode = SpatialObjectsDisplayNode;

  qvtkReconnect(oldDisplayNode, this->SpatialObjectsDisplayNode(), vtkCommand::ModifiedEvent,
    this, SLOT(updateWidgetFromMRML()));

  setSpatialObjectsDisplayNodeMode();//.....................................................ALERT
  this->updateWidgetFromMRML();
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::setSpatialObjectsDisplayNodeMode()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  d->SpatialObjectsDisplayNode->SetColorModeToScalarData();
  d->SpatialObjectsDisplayNode->SetActiveScalarName("TubeIDs");
  d->logic()->CreateTubeColorColorMap(d->SpatialObjectsNode, d->SpatialObjectsDisplayNode);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::updateMRMLFromWidget()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  qSlicerApplication * app = qSlicerApplication::application();
//  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
 // if (d->SelectTubesPushButton->isChecked())
//  {
//    interactionNode->SetCurrentInteractionMode(interactionNode->Place);
 // }
 // else
//  {
//    interactionNode->SetCurrentInteractionMode(interactionNode->ViewTransform);
//  }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

 // qSlicerApplication * app = qSlicerApplication::application();
 // vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
//  d->SelectTubesPushButton->setChecked(interactionNode->GetCurrentInteractionMode()== interactionNode->Place);

  if (!d->SpatialObjectsNode || !d->SpatialObjectsDisplayNode)
  {
    return;
  }
  else
  {
    int NumberOfTubes = d->logic()->GetSpatialObjectNumberOfTubes(d->SpatialObjectsNode);
    if (NumberOfTubes != d->TableWidget->rowCount())
    {
      setSpatialObjectsDisplayNodeMode();
      this->buildTubeDisplayTable();
    }
    return;
  }
}
// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::buildTubeDisplayTable()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->SpatialObjectsNode != 0)
  {
    this->setEnabled(1);
    //reset the table data
    d->TableWidget->setRowCount(0);
    //call function in logic to get the tube information
    //call logic
    std::vector<int> TubeIdList;
    std::vector<int> ParentIdList;
    std::vector<bool> IsRootList;
    d->logic()->GetSpatialObjectData(d->SpatialObjectsNode, TubeIdList, ParentIdList, IsRootList);
    if (TubeIdList.size() == 0)
    {
      qCritical("Error while reteriving Spatial Data !");
      return ;
    }

    QIcon pushButtonIcon;
    for (int i = 0; i < TubeIdList.size(); i++)
    {
      int newRow = d->TableWidget->rowCount();
      d->TableWidget->insertRow(newRow);
      //Tube ID Column
      int colIndex = d->columnIndex("Tube ID");
      QTableWidgetItem* tubeIDItem = new QTableWidgetItem();
      tubeIDItem->setText(QVariant(TubeIdList[i]).toString());
      tubeIDItem->setFlags(tubeIDItem->flags() &  ~Qt::ItemIsEditable);
      d->TableWidget->setItem(newRow, colIndex, tubeIDItem);

      //color column
      colIndex = d->columnIndex("Color");
      QColor TubeDisplayColor;
      getTubeDisplayColor(TubeDisplayColor, newRow);
      ctkColorPickerButton* colorPicker = new ctkColorPickerButton(this);
      colorPicker->setDisplayColorName(false);
      colorPicker->setColor(TubeDisplayColor);
      colorPicker->setWindowIconText(QVariant(TubeIdList[i]).toString());
      QObject::connect(colorPicker,
        SIGNAL(colorChanged(QColor)), this,
        SLOT(onClickTubeColorPicker(QColor)));
      d->TableWidget->setCellWidget(newRow, colIndex, colorPicker);

      //Is Root Column
      colIndex = d->columnIndex("Is Root");
      QTableWidgetItem* isRootItem = new QTableWidgetItem();
      if(IsRootList[i])
      {
        isRootItem->setText("Root");
        if(d->logic()->GetSpatialObjectChildrenData(d->SpatialObjectsNode, TubeIdList[i]).size() != 0)
        {      
          QPushButton* showTreePushButton = new QPushButton(this);
          showTreePushButton->setText("Root");
          QMenu *menu = new QMenu(this);
          QAction* colorAction = menu->addAction("Color Tree");
          colorAction->setData(QVariant(TubeIdList[i]));
          QObject::connect(colorAction,
            SIGNAL(triggered()), this,
            SLOT(onActionColorTree()));
          QAction* hideAction = menu->addAction("Hide Tree");
          hideAction->setData(QVariant(TubeIdList[i]));          
          QObject::connect(hideAction,
            SIGNAL(triggered()), this,
            SLOT(onActionHideTree()));
          QAction* displayAction = menu->addAction("Propogate Display Properties");
          displayAction->setData(QVariant(TubeIdList[i]));          
          QObject::connect(displayAction,
            SIGNAL(triggered()), this,
            SLOT(onActionDisplayPropertiesTree()));
          QAction* propogateOpacityAction = menu->addAction("Propogate Opacity");
          propogateOpacityAction->setData(QVariant(TubeIdList[i]));          
          QObject::connect(propogateOpacityAction,
            SIGNAL(triggered()), this,
            SLOT(onActionPropogateOpacityTree()));
          QAction* arteryAction = menu->addAction("Progogate Artery Status");
          arteryAction->setData(QVariant(TubeIdList[i]));          
          QObject::connect(arteryAction,
            SIGNAL(triggered()), this,
            SLOT(onActionPropogateArteryStatus()));

          showTreePushButton->setMenu(menu);
          d->TableWidget->setCellWidget(newRow, colIndex, showTreePushButton);
        }
      }
      else
      {
        isRootItem->setText("");
      }
      isRootItem->setFlags(isRootItem->flags() &  ~Qt::ItemIsEditable);
      d->TableWidget->setItem(newRow, colIndex, isRootItem);

      //Parent Id Column
      colIndex = d->columnIndex("Parent Id");
      QTableWidgetItem* parentIdItem = new QTableWidgetItem();
      parentIdItem->setText(QVariant(ParentIdList[i]).toString());   
      parentIdItem->setFlags(parentIdItem->flags() &  ~Qt::ItemIsEditable);
      d->TableWidget->setItem(newRow, colIndex, parentIdItem);
    }
    
  }
  return ;
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onTableCellClicked(QTableWidgetItem* item)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (item == 0)
  {
    return;
  }
}

// --------------------------------------------------------------------------
bool qSlicerInteractiveTubesToTreeTableWidget::getTubeDisplayColor(QColor &TubeDisplayColor, int row)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode)
  {
    qCritical("Error while reteriving Spatial Data tube display node !");
    return false;
  }
  double rgb[3];
  char colorMapName[80];
  strcpy(colorMapName, d->SpatialObjectsNode->GetName());
  strcat(colorMapName, "_TubeColor");
  vtkMRMLNode* colorNode1 = d->SpatialObjectsDisplayNode->GetScene()->GetFirstNodeByName(colorMapName);
  vtkMRMLProceduralColorNode* colorNode = vtkMRMLProceduralColorNode::SafeDownCast(colorNode1);
  if (colorNode != NULL)
  {
    vtkColorTransferFunction* colorMap = colorNode->GetColorTransferFunction();
    if (colorMap != NULL)
    {
      int tubeIDIndex = d->columnIndex("Tube ID");
      QTableWidgetItem *tubeIDItem = d->TableWidget->item(row, tubeIDIndex);
      int tubeID = tubeIDItem->text().toInt();

      colorMap->GetColor(tubeID, rgb);
      TubeDisplayColor = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
    }
    else
    {
      qCritical("No ColorMap");
    }
  }
  else
  {
    qCritical("No ColorNode");
  }
  return true;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onSelectTubeColorChanged(const QColor &color)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode)
  {
    return;
  }
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedIndexes();
  foreach (QModelIndex index, indexList)
  {
    ChangeTubeColor(color, -1, index.row());
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickTubeColorPicker(const QColor &color)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode)
  {
    return;
  }
  ctkColorPickerButton* colorPicker = qobject_cast<ctkColorPickerButton*>(sender());
  if(!colorPicker)
  {
    return;
  }
  bool isNumeric;
  int tubeID = colorPicker->windowIconText().toInt(&isNumeric);
  if (isNumeric)
  {
    char colorMapName[80];
    strcpy(colorMapName, d->SpatialObjectsNode->GetName());
    strcat(colorMapName, "_TubeColor");
    vtkMRMLNode* colorNode1 = d->SpatialObjectsDisplayNode->GetScene()->GetFirstNodeByName(colorMapName);
    vtkMRMLProceduralColorNode* colorNode = vtkMRMLProceduralColorNode::SafeDownCast(colorNode1);
    vtkColorTransferFunction* colorMap = colorNode->GetColorTransferFunction();
    colorMap->AddRGBPoint(tubeID, color.redF(), color.greenF(), color.blueF());
    d->SpatialObjectsDisplayNode->SetAndObserveColorNodeID(colorNode->GetID());
    d->logic()->SetSpatialObjectColor(d->SpatialObjectsNode, tubeID, color.redF(), color.greenF(), color.blueF());
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::ChangeTubeColor(const QColor &color, int tubeID, int rowID)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode)
  {
    return;
  }
  int colorIndex = d->columnIndex("Color");
  if (rowID >= 0)
  {    
    ctkColorPickerButton* t = qobject_cast<ctkColorPickerButton*>(d->TableWidget->cellWidget(rowID, colorIndex));
    t->setColor(color);
    return;
  }
  if(tubeID >=0)
  {
    //find row corresponding to this tube in the table
    int tubeIDIndex = d->columnIndex("Tube ID");
    for (int indexRow = 0; indexRow < d->TableWidget->rowCount(); indexRow++)
    {
      QTableWidgetItem* item = d->TableWidget->item(indexRow, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if (isNumeric && currTubeId == tubeID)
      {
        ctkColorPickerButton* t = qobject_cast<ctkColorPickerButton*>(d->TableWidget->cellWidget(indexRow, colorIndex));
        t->setColor(color);
        return;
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickHorizontalHeader(int column)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  d->TableWidget->sortByColumn(column);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickDeleteSelected()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
 
  if(d->SpatialObjectsNode->selectTubeIds.size() != 0)
  {
    d->logic()->deleteTubeFromSpatialObject(d->SpatialObjectsNode);
    buildTubeDisplayTable();
  }
  //delete corresponding markups.
  vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
  if(currentMarkupsNode)
  {
    int numMarups = currentMarkupsNode->GetNumberOfMarkups();
    for(int index = numMarups-1; index>=0; index--)
    {
      QString indexLabel = QString::fromStdString(currentMarkupsNode->GetNthMarkupLabel(index));
      bool isNumeric;
      int indexTubeId = indexLabel.toInt(&isNumeric);
      if(isNumeric && d->SpatialObjectsNode->selectTubeIds.find(indexTubeId) != d->SpatialObjectsNode->selectTubeIds.end())
      {
        currentMarkupsNode->RemoveMarkup(index);
      }
    }
  }
  return;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onTableSelectionChanged()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  //default color all the previously selected tubes
  for (std::set<int>::iterator it=d->SpatialObjectsNode->selectTubeIds.begin(); it!=d->SpatialObjectsNode->selectTubeIds.end(); ++it)
  {
    std::map<int, std::vector<int>>::iterator itDefaultColorMap;
    itDefaultColorMap = d->SpatialObjectsNode->DefaultColorMap.find(*it);
    if (itDefaultColorMap != d->SpatialObjectsNode->DefaultColorMap.end())
    {
      std::vector<int> color = d->SpatialObjectsNode->DefaultColorMap.find(*it)->second; 
      QColor defaultColor = QColor::fromRgbF(color[0], color[1], color[2]);
      ChangeTubeColor(defaultColor, *it, -1);
    }    
  }   
  //color the currently selected tubes
  d->SpatialObjectsNode->selectTubeIds.clear();
  int tubeIDIndex = d->columnIndex("Tube ID");
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedIndexes();
  foreach (QModelIndex index, indexList)
  {
    int curRowID = index.row();
    ChangeTubeColor(d->SelectTubeColorPicker->color(), -1, curRowID);
    QTableWidgetItem* item = d->TableWidget->item(curRowID, tubeIDIndex);
    bool isNumeric;
    int tubeID = item->text().toInt(&isNumeric);
    if(!isNumeric)
    {
      return;
    }
    d->SpatialObjectsNode->selectTubeIds.insert(tubeID);
  }
  return;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::hideMarkSelectedAsRootPushButton()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
//  d->MarkSelectedAsRootPushButton->setHidden(true);
//  d->MarkSelectedLabel->setHidden(true);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::hideColumn(int colID)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  d->TableWidget->hideColumn(colID);
}

//------------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidget::getColumnIndex(std::string columnName)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int index = d->columnIndex(QString(columnName.c_str()));
  return index;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::SelectTube(int tubeID, int rowID, bool isDefault)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int tubeIDIndex = d->columnIndex("Tube ID");
  if(tubeID < 0 && rowID < 0)
  {
    return;
  }
  if(tubeID < 0 && rowID >= 0)//Find TubeID for given RowID
  {
    QTableWidgetItem* item = d->TableWidget->item(rowID, tubeIDIndex);
    bool isNumeric;
    tubeID = item->text().toInt(&isNumeric);
    if(!isNumeric)
    {
      return;
    }
  }
  if(rowID < 0)// Find RowID for given TubeID
  {
    for (int indexRow = 0; indexRow < d->TableWidget->rowCount(); indexRow++)
    {
      QTableWidgetItem* item = d->TableWidget->item(indexRow, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if (isNumeric && currTubeId == tubeID)
      {
        rowID = indexRow;
        break;
      }
    }
  }
  if(rowID < 0 || tubeID < 0)
  {
    return;
  }
  if(isDefault)//un select the row and color corresponding tube in default color
  {
    std::map<int, std::vector<int>>::iterator itDefaultColorMap;
    itDefaultColorMap = d->SpatialObjectsNode->DefaultColorMap.find(tubeID);
    if (itDefaultColorMap != d->SpatialObjectsNode->DefaultColorMap.end())
    {
      std::vector<int> color = d->SpatialObjectsNode->DefaultColorMap.find(tubeID)->second; 
      QColor defaultColor = QColor::fromRgbF(color[0], color[1], color[2]);
      ChangeTubeColor(defaultColor, tubeID, rowID);
      if(this->isRowSelected(rowID,tubeID))
      {
        d->TableWidget->selectRow(rowID);
      }
    }   
  }
  else
  {//select the row and color corresponding tube in default color for selecting tube
    ChangeTubeColor(d->SelectTubeColorPicker->color(), -1, rowID);
    if(!this->isRowSelected(rowID,-1))
    {
      d->TableWidget->selectRow(rowID);
    }
  }  
}

//------------------------------------------------------------------------------
bool qSlicerInteractiveTubesToTreeTableWidget::isRowSelected(int rowID, int tubeID)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if(tubeID != -1)
  {
    int tubeIDIndex = d->columnIndex("Tube ID");
    for (int indexRow = 0; indexRow < d->TableWidget->rowCount(); indexRow++)
    {
      QTableWidgetItem* item = d->TableWidget->item(indexRow, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if (isNumeric && currTubeId == tubeID)
      {
        rowID = indexRow;
        break;
      }
    }
  }
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedIndexes();
  foreach (QModelIndex index, indexList)
  {
    if (rowID == index.row())
    {
      return true;
    }
  }
  return false;
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onNodeAddedEvent(vtkObject*, vtkObject* node)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->MarkupsNode == NULL)
  {
    vtkMRMLMarkupsNode* MarkupsNode1 = vtkMRMLMarkupsNode::SafeDownCast(node);
    if (MarkupsNode1)
    {
      d->MarkupsNode = MarkupsNode1;
      d->logic()->setActivePlaceNodeID(d->MarkupsNode);

      this->qvtkConnect(d->MarkupsNode, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,
        this, SLOT(onNthMarkupModifiedEvent(vtkObject*, vtkObject*)));
      this->qvtkConnect(d->MarkupsNode, vtkMRMLMarkupsNode::MarkupAddedEvent,
        this, SLOT(onMarkupAddEvent()));
      findTubeIDs(0);
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onNthMarkupModifiedEvent(vtkObject *caller, vtkObject *callData)
{
  if (caller == NULL || callData == NULL)
  {
    return;
  }

  int *nPtr = NULL;
  int n = -1;
  nPtr = reinterpret_cast<int *>(callData);
  if (nPtr)
  {
    n = *nPtr;
  }
  this->findTubeIDs(n);
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onMarkupAddEvent()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->MarkupsNode)
  {
    //to call NthMarkupAddedEvent explicitly.
    vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
    int num = currentMarkupsNode->GetNumberOfMarkups();
    currentMarkupsNode->SetNthMarkupVisibility(num - 1, false);
  }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::findTubeIDs(int index)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (d->MarkupsNode)
  {
    vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
    std::string currLabel = currentMarkupsNode->GetNthMarkupLabel(index);
    std::string currAssociatedNodeID = currentMarkupsNode->GetNthMarkupAssociatedNodeID(index);
    if (currAssociatedNodeID.find("vtkMRMLSpatialObjectsNode") == std::string::npos)
    {
      currentMarkupsNode->SetNthMarkupVisibility(index, false);
      return;
    }
    else
    {
      double xyz[3];
      currentMarkupsNode->GetMarkupPointLPS(index, 0, xyz);
      int TubeID = d->logic()->FindNearestTube(d->SpatialObjectsNode, xyz);
      if (TubeID == -1)
      {
        currentMarkupsNode->RemoveMarkup(index);
      }
      else
      {
        char newLabel[30];
        itoa(TubeID, newLabel, 10);
        for(int i = 0 ; i< index; i++)
        {
          std::string currMarkupLabel = currentMarkupsNode->GetNthMarkupLabel(i);
          bool isVisibleCurrMarkup = currentMarkupsNode->GetNthMarkupVisibility(i);
          if(i!= index && currMarkupLabel.compare(newLabel) == 0 && isVisibleCurrMarkup)
          {            
            currentMarkupsNode->RemoveMarkup(i);
            currentMarkupsNode->RemoveMarkup(index-1);
            SelectTube(TubeID, -1, true); 
            return;
          }
        }
        currLabel = currentMarkupsNode->GetNthMarkupLabel(index);
        if(currLabel.compare(newLabel) != 0)
        {
          currentMarkupsNode->SetNthMarkupLabel(index, newLabel);
        }
        if(!currentMarkupsNode->GetNthMarkupVisibility(index))
        {
          currentMarkupsNode->SetNthMarkupVisibility(index, true);
        }
        SelectTube(TubeID, -1, false);     
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::restoreDefaults()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
  
  if (d->SpatialObjectsNode != 0 && d->SpatialObjectsDisplayNode != 0)
  {
    int colorIndex = d->columnIndex("Color");
    int tubeIDIndex = d->columnIndex("Tube ID");
    int rowCount = d->TableWidget->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
      QTableWidgetItem* item1 = d->TableWidget->item(i, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item1->text().toInt(&isNumeric);
      if(isNumeric)
      {
        std::map<int, std::vector<int>>::iterator itDefaultColorMap;
        itDefaultColorMap = d->SpatialObjectsNode->DefaultColorMap.find(currTubeId);
        if (itDefaultColorMap != d->SpatialObjectsNode->DefaultColorMap.end())
        {
          std::vector<int> color = d->SpatialObjectsNode->DefaultColorMap.find(currTubeId)->second; 
          QColor defaultColor = QColor::fromRgbF(color[0], color[1], color[2]);
          ChangeTubeColor(defaultColor, -1, i);
        } 
      }
    }
    if(d->MarkupsNode)
    {
      d->MarkupsNode->RemoveAllMarkups();
    }
    d->TableWidget->clearSelection();
  }
  return;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onShowRootsColorChanged(const QColor &color)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode || !d->ShowRootsPushButton->isChecked())
  {
    return;
  }
  int isRootIndex = d->columnIndex("Is Root");
  int rowCount = d->TableWidget->rowCount();
  for(int rowIndex = 0; rowIndex < rowCount; rowIndex++)
  {
    QTableWidgetItem* item = d->TableWidget->item(rowIndex, isRootIndex);
    if(item->text() != QString(""))
    {
      ChangeTubeColor(color, -1, rowIndex);
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickShowRoots(bool value)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  QPushButton* pButton = qobject_cast<QPushButton*>(sender());
  QIcon pushButtonIcon;
  if(!pButton)
  {
    return;
  }

  if(value)
  {
    this->onShowRootsColorChanged(d->ShowRootsColorPicker->color());
    pushButtonIcon.addFile(QString::fromUtf8(":ColorDefault1.png"), QSize(), QIcon::Normal, QIcon::Off);
    pButton->setIcon(pushButtonIcon);
  }
  else
  {
    int isRootIndex = d->columnIndex("Is Root");
    int rowCount = d->TableWidget->rowCount();
    for(int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
      QTableWidgetItem* item = d->TableWidget->item(rowIndex, isRootIndex);
      if(item->text() != QString(""))
      {
        if(this->isRowSelected(rowIndex,-1))
        {
          SelectTube(-1, rowIndex, false);
        }
        else
        {
          SelectTube(-1, rowIndex, true);
        }
      }
    }
    pushButtonIcon.addFile(QString::fromUtf8(":ColorYellow1.png"), QSize(), QIcon::Normal, QIcon::Off);
    pButton->setIcon(pushButtonIcon);
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickSelectAllRoots()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int isRootIndex = d->columnIndex("Is Root");
  int rowCount = d->TableWidget->rowCount();
  for(int rowIndex = 0; rowIndex < rowCount; rowIndex++)
  {
    QTableWidgetItem* item = d->TableWidget->item(rowIndex, isRootIndex);
    if(item->text() != QString(""))
    {
      SelectTube(-1, rowIndex, false);
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onShowOrphansColorChanged(const QColor &color)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  if (!d->SpatialObjectsDisplayNode || !d->ShowOrphansPushButton->isChecked())
  {
    return;
  }

  int tubeIDIndex = d->columnIndex("Tube ID");
  for (int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++)
  {
    QTableWidgetItem* item = d->TableWidget->item(rowIndex, tubeIDIndex);
    bool isNumeric;
    int currTubeId = item->text().toInt(&isNumeric);
    if (isNumeric)
    {
      bool isSelected = false;
      if(d->logic()->GetSpatialObjectOrphanStatusData(d->SpatialObjectsNode, currTubeId))
      {
        ChangeTubeColor(d->ShowOrphansColorPicker->color(), -1, rowIndex);
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickShowOrphans(bool value)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  QPushButton* pButton = qobject_cast<QPushButton*>(sender());
  QIcon pushButtonIcon;
  if(!pButton)
  {
    return;
  }

  if(value)
  {
    this->onShowOrphansColorChanged(d->ShowRootsColorPicker->color());
    pushButtonIcon.addFile(QString::fromUtf8(":ColorDefault1.png"), QSize(), QIcon::Normal, QIcon::Off);
    pButton->setIcon(pushButtonIcon);
  }
  else
  {
    int tubeIDIndex = d->columnIndex("Tube ID");
    for (int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++)
    {
      QTableWidgetItem* item = d->TableWidget->item(rowIndex, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if (isNumeric)
      {
        if(d->logic()->GetSpatialObjectOrphanStatusData(d->SpatialObjectsNode, currTubeId))
        {
          if(this->isRowSelected(rowIndex,-1))
          {
            SelectTube(-1, rowIndex, false);
          }
          else
          {
            SelectTube(-1, rowIndex, true);
          }
        }
      }
    }
    pushButtonIcon.addFile(QString::fromUtf8(":ColorGreen1.png"), QSize(), QIcon::Normal, QIcon::Off);
    pButton->setIcon(pushButtonIcon);
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickSelectAllOrphans()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int tubeIDIndex = d->columnIndex("Tube ID");
  for (int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++)
  {
    QTableWidgetItem* item = d->TableWidget->item(rowIndex, tubeIDIndex);
    bool isNumeric;
    int currTubeId = item->text().toInt(&isNumeric);
    if (isNumeric)
    {
      if(d->logic()->GetSpatialObjectOrphanStatusData(d->SpatialObjectsNode, currTubeId))
      {
        SelectTube(-1, rowIndex, false);
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onActionColorTree()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  QAction* pAction = qobject_cast<QAction*>(sender());
  int tubeID;
  if(pAction)
  {
    tubeID = pAction->data().toInt();
  }
  else
  {
    return;
  }
  std::set<int> childrenIDList = d->logic()->GetSpatialObjectChildrenData(d->SpatialObjectsNode, tubeID);
  
  if(childrenIDList.size() != 0)
  {
    QColor rootColor;
    int tubeIDIndex = d->columnIndex("Tube ID");
    //find color of this tubeID i.e the root
    for(int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++)
    {
      QTableWidgetItem* item = d->TableWidget->item(rowIndex, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if(isNumeric && currTubeId == tubeID)
      {
        int colorIndex = d->columnIndex("Color");
        ctkColorPickerButton* t = qobject_cast<ctkColorPickerButton*>(d->TableWidget->cellWidget(rowIndex, colorIndex));
        rootColor = t->color();
        break;
      }
    }
    //color all the children   
    std::set<int>::iterator it;
    for(int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++)
    {
      QTableWidgetItem* item = d->TableWidget->item(rowIndex, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if (isNumeric)
      {
        it = childrenIDList.find(currTubeId);
        if(it != childrenIDList.end())
        {
          ChangeTubeColor(rootColor, -1, rowIndex);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickShowHideTubes(bool value)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  QPushButton* pButton = qobject_cast<QPushButton*>(sender());
  QIcon pushButtonIcon;
  if(!pButton)
  {
    return;
  }
  bool isNumeric;
  int tubeID = pButton->windowIconText().toInt(&isNumeric);
  if(isNumeric)
  {
    if(value) //hide it
    {

      pushButtonIcon.addFile(QString::fromUtf8(":Hide.png"), QSize(), QIcon::Normal, QIcon::Off);
      pButton->setIcon(pushButtonIcon);         
    }
    else //show it
    {
      
      pushButtonIcon.addFile(QString::fromUtf8(":Show.png"), QSize(), QIcon::Normal, QIcon::Off);
      pButton->setIcon(pushButtonIcon);
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::refreshTable()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
  
  if (d->SpatialObjectsNode != 0 && d->SpatialObjectsDisplayNode != 0)
  {
    int tubeIDIndex = d->columnIndex("Tube ID");
    for(int i = 0; i < d->TableWidget->rowCount(); i++)
    {
      QTableWidgetItem* item = d->TableWidget->item(i, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item->text().toInt(&isNumeric);
      if(isNumeric && d->SpatialObjectsNode->selectTubeIds.find(currTubeId) != d->SpatialObjectsNode->selectTubeIds.end())
      {
        d->TableWidget->selectRow(currTubeId);
      }
    }
  }
  return;
}