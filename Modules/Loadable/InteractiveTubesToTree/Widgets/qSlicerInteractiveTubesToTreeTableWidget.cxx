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
  this->columnLabels << "Tube ID" << "Show/Hide" << "Color" << "Is Root" << "Parent Id" << "Select As Root";
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
  pushButtonIcon.addFile(QString::fromUtf8(":AnnotationPointWithArrow.png"), QSize(), QIcon::Normal, QIcon::Off);
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
  pushButtonIcon.addFile(QString::fromUtf8(":AnnotationPointWithArrow.png"), QSize(), QIcon::Normal, QIcon::Off);
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

  QTableWidgetItem *selectedHeader = this->TableWidget->horizontalHeaderItem(this->columnIndex("Select As Root"));
  selectedHeader->setToolTip(QString("Click in this column to select/deselect a tube as Root"));

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
  this->MarkSelectedAsRootPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->MarkSelectedAsRootPushButton, SIGNAL(clicked()),
    q, SLOT(onClickMarkSelectedAsRoot()));

  pushButtonIcon.addFile(QString::fromUtf8(":DeleteSelected.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->DeleteSelectedPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->DeleteSelectedPushButton, SIGNAL(clicked()),
    q, SLOT(onClickDeleteSelected()));

  pushButtonIcon.addFile(QString::fromUtf8(":SelectAllRoots.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectAllRootsPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->SelectAllRootsPushButton, SIGNAL(clicked()),
    q, SLOT(onClickSelectAllRoots()));

  pushButtonIcon.addFile(QString::fromUtf8(":SelectAllOrphans.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectAllOrphansPushButton->setIcon(pushButtonIcon);
  QObject::connect(this->SelectAllOrphansPushButton, SIGNAL(clicked()),
    q, SLOT(onClickSelectAllOrphans()));

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
  qCritical("\nIn set Spatial Object Node\n");
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
  buildDefaultColorMap();

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

    QSignalMapper* signalMapper = new QSignalMapper (this) ;
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

      //show/Hide Column
      colIndex = d->columnIndex("Show/Hide");
      QPushButton* showHidePushButton = new QPushButton(this);
      QIcon pushButtonIcon;
      pushButtonIcon.addFile(QString::fromUtf8(":Show.png"), QSize(), QIcon::Normal, QIcon::Off);
      showHidePushButton->setIcon(pushButtonIcon);
      showHidePushButton->setCheckable(true);
      showHidePushButton->setWindowIconText(QVariant(TubeIdList[i]).toString());
      QObject::connect(showHidePushButton,
        SIGNAL(toggled(bool)),
        this, SLOT(onClickShowHideTubes(bool)));
      d->TableWidget->setCellWidget(newRow, colIndex, showHidePushButton);

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
        QPushButton* showTreePushButton = new QPushButton(this);
        showTreePushButton->setText("Root");
        pushButtonIcon.addFile(QString::fromUtf8(":ShowTree.png"), QSize(), QIcon::Normal, QIcon::Off);
        showTreePushButton->setIcon(pushButtonIcon);
        QMenu *menu = new QMenu(this);
        QAction* action = menu->addAction("Color Tree Same As Root");
        showTreePushButton->setMenu(menu);
        connect (action, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
        signalMapper->setMapping (action, TubeIdList[i]) ;
        d->TableWidget->setCellWidget(newRow, colIndex, showTreePushButton);
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

      // Selected Column
      colIndex = d->columnIndex("Select As Root");
      QTableWidgetItem* selectedItem = new QTableWidgetItem();
      selectedItem->setCheckState(Qt::Unchecked);
      selectedItem->setFlags(selectedItem->flags() &  ~Qt::ItemIsEditable);
      selectedItem->setData(Qt::DisplayRole, "");      
      d->TableWidget->setItem(newRow, colIndex, selectedItem);
    }
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(onActionColorTree(int))) ;
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
  int column = item->column();
  if (column == d->columnIndex(QString("Select As Root")) )
  {
    // toggle the user role, the icon update is triggered by this change
    if (item->checkState() == QVariant(false))
    {
      item->setCheckState(Qt::Checked);
      item->setData(Qt::DisplayRole, "Root");
    }
    else
    {
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::DisplayRole, "");
    }
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
std::string qSlicerInteractiveTubesToTreeTableWidget::getSelectedRootIds()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  std::string selectedRoodtIds = "";
  int selectedIndex = d->columnIndex("Select As Root");
  int rowCount = d->TableWidget->rowCount();
  for (int row = 0; row < rowCount; row++)
  {
    QTableWidgetItem *itemSelectRootId = d->TableWidget->item(row, selectedIndex);
    if (itemSelectRootId->checkState() == Qt::Checked)
    {
      int tubeIDIndex = d->columnIndex("Tube ID");
      QTableWidgetItem *itemTubeId = d->TableWidget->item(row, tubeIDIndex);
      selectedRoodtIds += itemTubeId->text().toStdString();
      selectedRoodtIds += ", ";
    }
  }
  return selectedRoodtIds;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickHorizontalHeader(int column)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
  if (column == d->columnIndex(QString("Select As Root")))
  {
    d->TableWidget->sortByColumn(column, Qt::DescendingOrder);
  }
  d->TableWidget->sortByColumn(column);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickMarkSelectedAsRoot()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int selectedIndex = d->columnIndex("Select As Root");
  QList<QTableWidgetItem *> selectedItems = d->TableWidget->selectedItems();
  QTableWidgetItem * item;
  for each (item in selectedItems)
  {
    int currRow = item->row();
    int currColumn = item->column();
    if (currColumn == selectedIndex)
    {
      item->setCheckState(Qt::Checked);
      item->setData(Qt::DisplayRole, "Root");
    }
  }
  return;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickDeleteSelected()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);

  int tubeIDIndex = d->columnIndex("Tube ID");
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedIndexes();
  std::set<int> selectedTubeID;
  foreach (QModelIndex index, indexList)
  {
    int curRow = index.row();
    QTableWidgetItem* item = d->TableWidget->item(curRow, tubeIDIndex);
    bool isNumeric;
    int tubeID = item->text().toInt(&isNumeric);
    if(isNumeric)
    {
      selectedTubeID.insert(tubeID);      
    }  
  } 
  if(selectedTubeID.size() != 0)
  {
    d->logic()->deleteTubeFromSpatialObject(d->SpatialObjectsNode, selectedTubeID);
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
      if(isNumeric && selectedTubeID.find(indexTubeId) != selectedTubeID.end())
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
/*
  vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
  if(!currentMarkupsNode)
  {
    return;
  }

  int numMarups = currentMarkupsNode->GetNumberOfMarkups();
  int tubeIDIndex = d->columnIndex("Tube ID");
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedIndexes();

  for(int index = 0; index<numMarups; index++)
  {
    int flag = 0;
    std::string indexLabel = currentMarkupsNode->GetNthMarkupLabel(index);
    foreach (QModelIndex index, indexList)
    {
      int curRow = index.row();
      QTableWidgetItem* item = d->TableWidget->item(curRow, tubeIDIndex);
      std::string currLabel = item->text().toStdString();
      if(currLabel.compare(indexLabel) == 0)
      {
        flag = 1;
        break;
      }
    }
    if(flag == 0)
    {
      QString label = QString::fromStdString(indexLabel);
      bool isNumeric;
      int indexTubeId = label.toInt(&isNumeric);
      if(isNumeric)
      {
        currentMarkupsNode->RemoveMarkup(index);
        selectRow(-1, indexTubeId, true);
      }
    }
  }

  return;
  */
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::hideMarkSelectedAsRootPushButton()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
  d->MarkSelectedAsRootPushButton->setHidden(true);
  d->MarkSelectedLabel->setHidden(true);
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
  if(isDefault)//un select the row and color corresponding tube in default color
  {
    std::vector<int> color = this->defaultColorMap.find(tubeID)->second;
    QColor defaultColor = QColor::fromRgbF(color[0], color[1], color[2]);
    ChangeTubeColor(defaultColor, -1, rowID);
    if(this->isRowSelected(rowID,-1))
    {
      d->TableWidget->selectRow(rowID);
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
    int rootIndex = d->columnIndex("Select As Root");
    int colorIndex = d->columnIndex("Color");
    int tubeIDIndex = d->columnIndex("Tube ID");
    int rowCount = d->TableWidget->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
      QTableWidgetItem* item = d->TableWidget->item(i, rootIndex);
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::DisplayRole, "");
      QTableWidgetItem* item1 = d->TableWidget->item(i, tubeIDIndex);
      bool isNumeric;
      int currTubeId = item1->text().toInt(&isNumeric);
      if(isNumeric)
      {
        std::vector<int> color = this->defaultColorMap.find(currTubeId)->second;
        QColor defaultColor = QColor::fromRgbF(color[0], color[1], color[2]);
        ChangeTubeColor(defaultColor, -1, i);
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
  if(value)
  {
    this->onShowRootsColorChanged(d->ShowRootsColorPicker->color());
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
  if(value)
  {
    this->onShowOrphansColorChanged(d->ShowRootsColorPicker->color());
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
void qSlicerInteractiveTubesToTreeTableWidget::onActionColorTree(int tubeID)
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
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

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::buildDefaultColorMap()
{
  Q_D(qSlicerInteractiveTubesToTreeTableWidget);
  if (d->SpatialObjectsNode != 0)
  {
    d->logic()->buildDefaultColorMap(d->SpatialObjectsNode, this->defaultColorMap);
  }
}