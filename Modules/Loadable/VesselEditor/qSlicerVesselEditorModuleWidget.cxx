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
#include "qSlicerVesselEditorModuleWidget.h"
#include "vtkSlicerVesselEditorLogic.h"
#include "ui_qSlicerVesselEditorModuleWidget.h"
#include "qSlicerInteractiveTubesToTreeTableWidget.h"

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"
#include "vtkMRMLScene.h"
#include <vtkMRMLMarkupsNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVesselEditorModuleWidgetPrivate: public Ui_qSlicerVesselEditorModuleWidget
{
  Q_DECLARE_PUBLIC( qSlicerVesselEditorModuleWidget );
protected:
  qSlicerVesselEditorModuleWidget* const q_ptr;
public:
  qSlicerVesselEditorModuleWidgetPrivate( qSlicerVesselEditorModuleWidget& object );
  ~qSlicerVesselEditorModuleWidgetPrivate();
  vtkSlicerVesselEditorLogic* logic() const;

  void init();
  vtkMRMLSpatialObjectsNode* inputSpatialObject;
};

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModuleWidgetPrivate::qSlicerVesselEditorModuleWidgetPrivate(
  qSlicerVesselEditorModuleWidget& object ) : q_ptr( &object )
{
  this->inputSpatialObject = 0;
}

//-----------------------------------------------------------------------------
qSlicerVesselEditorModuleWidgetPrivate::~qSlicerVesselEditorModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerVesselEditorLogic* qSlicerVesselEditorModuleWidgetPrivate::logic() const
{
	Q_Q( const qSlicerVesselEditorModuleWidget );
	return vtkSlicerVesselEditorLogic::SafeDownCast( q->logic() );
}

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModuleWidget::qSlicerVesselEditorModuleWidget( QWidget* _parent )
  : Superclass( _parent )
  , d_ptr( new qSlicerVesselEditorModuleWidgetPrivate( *this ) )
{
}

//-----------------------------------------------------------------------------
qSlicerVesselEditorModuleWidget::~qSlicerVesselEditorModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::setup()
{
  Q_D( qSlicerVesselEditorModuleWidget );
  d->init();
  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidgetPrivate::init()
{
  Q_Q( qSlicerVesselEditorModuleWidget );

  this->setupUi( q );

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ),
    q, SLOT( setInputSpatialObjectsNode( vtkMRMLNode* ) ) );

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this->Table, SLOT(setSpatialObjectsNode(vtkMRMLNode*)));

  QIcon pushButtonIcon;
  pushButtonIcon.addFile(QString::fromUtf8(":Connect.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->ConnectPushButton->setIcon(pushButtonIcon);
  pushButtonIcon.addFile(QString::fromUtf8(":Disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->DisconnectPushButton->setIcon(pushButtonIcon);
  pushButtonIcon.addFile(QString::fromUtf8(":Select.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SelectTubeToSplitPushButton->setIcon(pushButtonIcon);
  pushButtonIcon.addFile(QString::fromUtf8(":Split.png"), QSize(), QIcon::Normal, QIcon::Off);
  this->SplitPushButton->setIcon(pushButtonIcon);
  
  
  QObject::connect(
    this->ConnectSourceTubePushButton, SIGNAL( clicked() ),
    q, SLOT( onClickConnectSourceTube() ) );
  QObject::connect(
    this->ConnectTargetTubePushButton, SIGNAL( clicked() ),
    q, SLOT( onClickConnectTargetTube() ) );
  QObject::connect(
    this->DisconnectSourceTubePushButton, SIGNAL( clicked() ),
    q, SLOT( onClickDisconnectSourceTube() ) );
  QObject::connect(
    this->DisconnectTargetTubePushButton, SIGNAL( clicked() ),
    q, SLOT( onClickDisconnectTargetTube() ) );

  QObject::connect(
    this->SelectTubeToSplitPushButton, SIGNAL( clicked() ),
    q, SLOT( onClickSelectTubeToSplit() ) );

  QObject::connect(
    this->SplitPushButton, SIGNAL( clicked() ),
    q, SLOT( onApplySplitTube() ) );
  QObject::connect(
    this->ConnectPushButton, SIGNAL( clicked() ),
    q, SLOT( onApplyConnectTubes() ) );
  QObject::connect(
    this->DisconnectPushButton, SIGNAL( clicked() ),
    q, SLOT( onApplyDisconnectTubes() ) );

  QObject::connect(
    this->RestoreDefaultsPushButton, SIGNAL( clicked() ),
    q, SLOT( restoreDefaults() ) );
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onEnter()
{
  Q_D(qSlicerVesselEditorModuleWidget);
  if (this->mrmlScene() == 0)
  {
    return;
  }
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
   d->Table, SLOT(onNodeAddedEvent(vtkObject*, vtkObject*)));
  vtkMRMLNode* vtkMRMLMarkupFiducialNode = this->mrmlScene()->GetNthNodeByClass(0,"vtkMRMLMarkupsFiducialNode");
  if(vtkMRMLMarkupFiducialNode)
  {
    d->Table->onNodeAddedEvent(NULL, vtkMRMLMarkupFiducialNode);
  }
  d->Table->buildTubeDisplayTable();
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget
::setInputSpatialObjectsNode( vtkMRMLNode* node )
{
  this->setInputSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast( node ) );
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget
::setInputSpatialObjectsNode( vtkMRMLSpatialObjectsNode* node )
{
  Q_D( qSlicerVesselEditorModuleWidget );

  if ( d->inputSpatialObject == node )
    {
      return;
    }
  d->inputSpatialObject = node;
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::restoreDefaults()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  d->ApplyPushButton->setEnabled( d->inputSpatialObject != 0 );
  d->Table->restoreDefaults();
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onApplyConnectTubes()
{
  Q_D( qSlicerVesselEditorModuleWidget );
  bool isNumeric;
  int primaryTubeId = d->ConnectSourceTubePushButton->text().toInt(&isNumeric);
  if(isNumeric)
  {
    int secondaryTubeId = d->ConnectTargetTubePushButton->text().toInt(&isNumeric);
    if(secondaryTubeId)
    {
      d->logic()->ConnectTubesInSpatialObject(d->inputSpatialObject, primaryTubeId, secondaryTubeId);
      d->ConnectSourceTubePushButton->setText("Source Tube");
      d->ConnectTargetTubePushButton->setText("Target Tube");
    }
    else
    {
      qDebug() << " Select a Secondary Tube first.";
    }
  }
  else
  {
    qDebug() << " Select a Primary Tube first.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onApplyDisconnectTubes()
{
  Q_D( qSlicerVesselEditorModuleWidget );
  bool isNumeric;
  int primaryTubeId = d->DisconnectSourceTubePushButton->text().toInt(&isNumeric);
  if(isNumeric)
  {
    int secondaryTubeId = d->DisconnectTargetTubePushButton->text().toInt(&isNumeric);
    if(secondaryTubeId)
    {
      d->logic()->DisconnectTubesInSpatialObject(d->inputSpatialObject, primaryTubeId, secondaryTubeId);
      d->DisconnectSourceTubePushButton->setText("Source Tube");
      d->DisconnectTargetTubePushButton->setText("Target Tube");
    }
    else
    {
      qDebug() << " Select a Secondary Tube first.";
    }
  }
  else
  {
    qDebug() << " Select a Primary Tube first.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onApplySplitTube()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  bool isNumeric;
  int tubeId = d->SelectTubeToSplitPushButton->text().toInt(&isNumeric);
  if(isNumeric)
  {
    vtkMRMLMarkupsNode* MarkupsNode = d->Table->getMRMLMarkupsNode();
    if (MarkupsNode)
    {
      int numMarkups = MarkupsNode->GetNumberOfMarkups();
      for(int index = numMarkups-1; index>=0; index--)
      {
        QString indexLabel = QString::fromStdString(MarkupsNode->GetNthMarkupLabel(index));
        bool isNumeric;
        int indexTubeId = indexLabel.toInt(&isNumeric);
        if(isNumeric && indexTubeId == tubeId)
        {
          double xyz[3];
          MarkupsNode->GetMarkupPointLPS(index, 0, xyz);
          d->logic()->SplitTubeInSpatialObject(d->inputSpatialObject, tubeId, xyz);
          int i = 0;
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onClickConnectSourceTube()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();
  if( selectedTubeIds.size() == 1)
  {
    std::set<int>::iterator it=selectedTubeIds.begin();   
    d->ConnectSourceTubePushButton->setText(QString(std::to_string(*it).c_str()));
  }
  else
  {
    d->ConnectSourceTubePushButton->setText("Source Tube");
    qDebug() << " Select Only One Tube.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onClickConnectTargetTube()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();
  if( selectedTubeIds.size() == 1)
  {
    std::set<int>::iterator it=selectedTubeIds.begin();
    d->ConnectTargetTubePushButton->setText(QString(std::to_string(*it).c_str()));
  }
  else
  {
    d->ConnectTargetTubePushButton->setText("Target Tube");
    qDebug() << " Select Only One Tube.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onClickDisconnectSourceTube()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();
  if( selectedTubeIds.size() == 1)
  {
    std::set<int>::iterator it=selectedTubeIds.begin();   
    d->DisconnectSourceTubePushButton->setText(QString(std::to_string(*it).c_str()));
  }
  else
  {
    d->DisconnectSourceTubePushButton->setText("Source Tube");
    qDebug() << " Select Only One Tube.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onClickDisconnectTargetTube()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();
  if( selectedTubeIds.size() == 1)
  {
    std::set<int>::iterator it=selectedTubeIds.begin();
  }
  else
  {
    d->DisconnectSourceTubePushButton->setText("Target Tube");
    qDebug() << " Select Only One Tube.";
  }
}

//------------------------------------------------------------------------------
void qSlicerVesselEditorModuleWidget::onClickSelectTubeToSplit()
{
  Q_D( qSlicerVesselEditorModuleWidget );

  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();
  if( selectedTubeIds.size() == 1)
  {
    std::set<int>::iterator it=selectedTubeIds.begin();
    d->SelectTubeToSplitPushButton->setText(QString(std::to_string(*it).c_str()));
    d->SelectTubeToSplitPushButton->setIcon(QIcon());
  }
  else
  {
    QIcon pushButtonIcon;
    pushButtonIcon.addFile(QString::fromUtf8(":Select.png"), QSize(), QIcon::Normal, QIcon::Off);
    d->SelectTubeToSplitPushButton->setIcon(pushButtonIcon);
    d->SplitTubeLabel->setText("");
    qDebug() << " Select Only One Tube.";
  }
}
