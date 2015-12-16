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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerInteractiveTubesToTreeModuleWidget.h"
#include "vtkSlicerInteractiveTubesToTreeLogic.h"
#include "ui_qSlicerInteractiveTubesToTreeModuleWidget.h"

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLSelectionNode.h>
#include "vtkMRMLScene.h"
#include <vtkMRMLMarkupsNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerInteractiveTubesToTreeModuleWidgetPrivate: public Ui_qSlicerInteractiveTubesToTreeModuleWidget
{

  Q_DECLARE_PUBLIC(qSlicerInteractiveTubesToTreeModuleWidget);
protected:
  qSlicerInteractiveTubesToTreeModuleWidget* const q_ptr;
public:
  qSlicerInteractiveTubesToTreeModuleWidgetPrivate(qSlicerInteractiveTubesToTreeModuleWidget& object);
  ~qSlicerInteractiveTubesToTreeModuleWidgetPrivate();
  vtkSlicerInteractiveTubesToTreeLogic* logic() const;

  void init();
  vtkMRMLSpatialObjectsNode* inputSpatialObject;
  vtkMRMLSpatialObjectsNode* outputSpatialObject;
  vtkMRMLMarkupsNode* MarkupsNode;
};

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::qSlicerInteractiveTubesToTreeModuleWidgetPrivate(
	qSlicerInteractiveTubesToTreeModuleWidget& object) : q_ptr(&object)
{
  this->inputSpatialObject = 0;
  this->outputSpatialObject = 0;
  this->MarkupsNode = 0;
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::~qSlicerInteractiveTubesToTreeModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic* qSlicerInteractiveTubesToTreeModuleWidgetPrivate::logic() const
{
	Q_Q(const qSlicerInteractiveTubesToTreeModuleWidget);
	return vtkSlicerInteractiveTubesToTreeLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidget::qSlicerInteractiveTubesToTreeModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerInteractiveTubesToTreeModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidget methods
//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidget::~qSlicerInteractiveTubesToTreeModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setup()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  d->init();

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidgetPrivate::init()
{
  Q_Q(qSlicerInteractiveTubesToTreeModuleWidget);

  this->setupUi(q);
  //Picking root tube ids

  QIcon pickRootTubesPushButtonIcon;
  pickRootTubesPushButtonIcon.addFile(QString::fromUtf8(":AnnotationPointWithArrow.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->PickRootTubesPushButton->setIcon(pickRootTubesPushButtonIcon);

  QObject::connect(
    this->PickRootTubesPushButton, SIGNAL(toggled(bool)),
    q, SLOT(updateMRMLFromWidget()));

  qSlicerApplication * app = qSlicerApplication::application();
  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
  q->qvtkReconnect(interactionNode, vtkCommand::ModifiedEvent,
    q, SLOT(updateWidgetFromMRML()));

  //REST.....
  QRegExp rx("[0-9]+([0-9]*[ ]*,[ ]*)*");
  QValidator *validator = new QRegExpValidator(rx);
  this->RootTubeIDListLineEdit->setValidator(validator);

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setInputSpatialObjectsNode(vtkMRMLNode*)));

  QObject::connect(
    this->OutputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setOutputSpatialObjectsNode(vtkMRMLNode*)));

  QObject::connect(
    this->RestoreDefaultsPushButton, SIGNAL(clicked()),
    q, SLOT(restoreDefaults()));

  QObject::connect(
    this->ApplyPushButton, SIGNAL(clicked()),
    q, SLOT(runConversion()));

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this->Table, SLOT(setSpatialObjectsNode(vtkMRMLNode*)));


  q->updateWidgetFromMRML();
}
//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::onEnter()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  if (this->mrmlScene() == 0)
  {
    return;
  }
  updateWidgetFromMRML();

  qSlicerApplication * app = qSlicerApplication::application();
  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
  interactionNode->SetPlaceModePersistence(1);

  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
    this, SLOT(onNodeAddedEvent(vtkObject*, vtkObject*)));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setInputSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setInputSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setInputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->inputSpatialObject == node)
  {
    return;
  }
  d->inputSpatialObject = node;
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setOutputSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setOutputSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setOutputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
	Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->outputSpatialObject == node)
  {
    return;
  }
  d->outputSpatialObject = node;
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::restoreDefaults()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  d->MaxContinuityAngleErrorSliderWidget->setValue(180);
  d->MaxTubeDistanceToRadiusSliderWidget->setValue(2);
  d->RemoveOrphanTubesCheckBox->setChecked(false);
  d->RootTubeIDListLineEdit->setText("");
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::runConversion()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  double maxTubeDistanceToRadiusRatio = d->MaxTubeDistanceToRadiusSliderWidget->value();
  double maxContinuityAngleError = d->MaxContinuityAngleErrorSliderWidget->value();
  bool removeOrphanTubes = d->RemoveOrphanTubesCheckBox->isChecked();

  //getting root ids from the text box
  std::string rootTubeIdList = d->RootTubeIDListLineEdit->text().toStdString();
  //getting root ids from the table
  std::string selectedRoodtIds = d->Table->getSelectedRootIds();

  rootTubeIdList = rootTubeIdList + " ," + selectedRoodtIds;

  d->ApplyPushButton->setEnabled(false);

  if (!d->logic()->Apply(d->inputSpatialObject, d->outputSpatialObject, maxTubeDistanceToRadiusRatio,
    maxContinuityAngleError, removeOrphanTubes, rootTubeIdList))
  {
    qCritical("Error while running conversion !");
  }

  d->ApplyPushButton->setChecked(false);
  d->ApplyPushButton->setEnabled(true);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::updateMRMLFromWidget()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  qSlicerApplication * app = qSlicerApplication::application();
  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();
  if (d->PickRootTubesPushButton->isChecked())
  {
    interactionNode->SetCurrentInteractionMode(interactionNode->Place);
  }
  else
  {
    interactionNode->SetCurrentInteractionMode(interactionNode->ViewTransform);
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  qSlicerApplication * app = qSlicerApplication::application();
  vtkMRMLInteractionNode* interactionNode = app->applicationLogic()->GetInteractionNode();

  d->PickRootTubesPushButton->setChecked(interactionNode->GetCurrentInteractionMode()== interactionNode->Place);
}

// --------------------------------------------------------------------------
vtkMRMLSpatialObjectsNode* qSlicerInteractiveTubesToTreeModuleWidget::mrmlSpatialObjectNode()const
{
  Q_D(const qSlicerInteractiveTubesToTreeModuleWidget);
  return d->inputSpatialObject;
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::findTubeIDs(int index)
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
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
      int TubeID = d->logic()->FindNearestTube(d->inputSpatialObject, xyz);
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
            d->Table->selectRow(TubeID,true);  
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

        d->Table->selectRow(TubeID,false);     
      }
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::onNthMarkupModifiedEvent(vtkObject *caller, vtkObject *callData)
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
void qSlicerInteractiveTubesToTreeModuleWidget::onMarkupAddEvent()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->MarkupsNode)
  {
    vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
    int num = currentMarkupsNode->GetNumberOfMarkups();
    currentMarkupsNode->SetNthMarkupVisibility(num - 1, false);
  }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::onNodeAddedEvent(vtkObject*, vtkObject* node)
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (!this->mrmlScene())
  {
    return;
  }
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