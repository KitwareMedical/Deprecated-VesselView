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
#include "qSlicerInteractiveTubesToTreeModuleWidget.h"
#include "ui_qSlicerInteractiveTubesToTreeModuleWidget.h"

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkSlicerInteractiveTubesToTreeLogic.h"

// TubeTK includes
#include "tubeTubeMath.h"
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
  vtkMRMLSpatialObjectsNode* currentSpatialObject;
  vtkMRMLVolumeNode* volumeNode;
};

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::qSlicerInteractiveTubesToTreeModuleWidgetPrivate(
	qSlicerInteractiveTubesToTreeModuleWidget& object) : q_ptr(&object)
{
  this->currentSpatialObject = 0;
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::~qSlicerInteractiveTubesToTreeModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidget methods
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

  QIcon pickRootTubesPushButtonIcon;
  pickRootTubesPushButtonIcon.addFile(QString::fromUtf8(":AnnotationPointWithArrow.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->PickRootTubesPushButton->setIcon(pickRootTubesPushButtonIcon);

  QRegExp rx("[0-9]+([0-9]*[ ]*,[ ]*)*");
  QValidator *validator = new QRegExpValidator(rx);
  this->RootTubeIDListLineEdit->setValidator(validator);

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setCurrentSpatialObjectsNode(vtkMRMLNode*)));

  QObject::connect(
    this->OutputVolumeNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setVolumeNode(vtkMRMLNode*)));

  QObject::connect(
    this->RestoreDefaultsPushButton, SIGNAL(clicked()),
    q, SLOT(restoreDefaults()));
  
  QObject::connect(
    this->ApplyPushButton, SIGNAL(clicked()),
    q, SLOT(runConversion()));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setCurrentSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setCurrentSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setCurrentSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->currentSpatialObject == node)
  {
    return;
  }
  d->currentSpatialObject = node;
  d->ApplyPushButton->setEnabled(d->volumeNode != 0 && d->currentSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setVolumeNode(vtkMRMLNode* node)
{
	this->setVolumeNode(vtkMRMLVolumeNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setVolumeNode(vtkMRMLVolumeNode* volumeNode)
{
	Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

	d->volumeNode = volumeNode;
  d->ApplyPushButton->setEnabled(d->volumeNode != 0 && d->currentSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::restoreDefaults()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  d->MaxContinuityAngleErrorSliderWidget->setValue(180);
  d->MaxTubeDistanceToRadiusSliderWidget->setValue(2);
  d->RemoveOrphanTubesCheckBox->setChecked(false);
  d->RootTubeIDListLineEdit->setText("");
  d->ApplyPushButton->setEnabled(d->volumeNode != 0 && d->currentSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::runConversion()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
 
  double maxTubeDistanceToRadiusRatio = d->MaxTubeDistanceToRadiusSliderWidget->value();
  double maxContinuityAngleError = d->MaxContinuityAngleErrorSliderWidget->value();
  bool removeOrphanTubes = d->RemoveOrphanTubesCheckBox->isChecked();

  std::vector<int> rootTubeIdList;
  QString rootTubeIdString = d->RootTubeIDListLineEdit->text();
  std::string temp = rootTubeIdString.toStdString();
  QStringList rootTubeIdStringList = rootTubeIdString.split(",");
  for (int i = 0; i < rootTubeIdStringList.size(); ++i)
  {
    bool isNumeric;
    int rootTubeId = rootTubeIdStringList[i].toInt(&isNumeric);
    if (isNumeric)
      rootTubeIdList.push_back(rootTubeId);
  }

  d->ApplyPushButton->setEnabled(false);

  if (!d->logic()->Apply(d->currentSpatialObject, d->volumeNode, maxTubeDistanceToRadiusRatio, 
    maxContinuityAngleError, removeOrphanTubes, temp))
  {
    qCritical("Error while running conversion !");
  }

  d->ApplyPushButton->setChecked(false);
  d->ApplyPushButton->setEnabled(true);
}