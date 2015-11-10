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
#include "vtkMRMLScene.h"
#include <vtkMRMLMarkupsFiducialNode.h>

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

};

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::qSlicerInteractiveTubesToTreeModuleWidgetPrivate(
	qSlicerInteractiveTubesToTreeModuleWidget& object) : q_ptr(&object)
{
  this->inputSpatialObject = 0;
  this->outputSpatialObject = 0;
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
  findTubeIDs();
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
void qSlicerInteractiveTubesToTreeModuleWidget::findTubeIDs()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  std::vector<vtkMRMLNode*> nodes;
  int numMarkups = this->mrmlScene()->GetNumberOfNodesByClass("vtkMRMLMarkupsNode");
  if (numMarkups > 0)
  {
    this->mrmlScene()->GetNodesByClass("vtkMRMLMarkupsNode", nodes);
    vtkMRMLMarkupsNode* currentMarkupsNode = vtkMRMLMarkupsNode::SafeDownCast(nodes[0]);
    d->logic()->setActivePlaceNodeID(currentMarkupsNode);
    
    int num = currentMarkupsNode->GetNumberOfMarkups();
    for (int i = 0; i < num; i++)
    {
      std::string currLabel = currentMarkupsNode->GetNthMarkupLabel(i);
      if (currLabel.find("None") != std::string::npos)
      {
        currentMarkupsNode->SetNthMarkupVisibility(i, 0);
        continue;
      }
      if (currLabel.find("TubeId") == std::string::npos)
      {
        std::string currAssociatedNodeID = currentMarkupsNode->GetNthMarkupAssociatedNodeID(i);
        if (currAssociatedNodeID.find("vtkMRMLSpatialObjectsNode") == std::string::npos)
        {
          currentMarkupsNode->SetNthMarkupLabel(i, "None");
          continue;
        }
        else
        {
          double xyz[3];
          currentMarkupsNode->GetMarkupPointLPS(i, 0, xyz);
        //  currentMarkupsNode->GetMarkupPoint(i, 0, xyz);
          int TubeID = d->logic()->FindNearestTube(d->inputSpatialObject, xyz);
          if (TubeID == -1)
          {
            currentMarkupsNode->SetNthMarkupLabel(i, "None");
          }
          else
          {
            char TubeId[30], newLabel[30];
            itoa(TubeID, TubeId, 10);
            strcpy(newLabel, "TubeId-");
            strcat(newLabel, TubeId);
            currentMarkupsNode->SetNthMarkupLabel(i, newLabel);
          }
        }
      }
    }
  }
}
