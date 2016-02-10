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

  QRegExp rx( "[0-9]+([0-9]*[ ]*,[ ]*)*" );
  QValidator *validator = new QRegExpValidator( rx );
  this->MergeVesselsLineEdit->setValidator( validator );
  this->DeleteVesselsLineEdit->setValidator( validator );

  QObject::connect(
    this->RestoreDefaultsPushButton, SIGNAL( clicked() ),
    q, SLOT( restoreDefaults() ) );

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this->Table, SLOT(setSpatialObjectsNode(vtkMRMLNode*)));
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

  d->MergeVesselsLineEdit->setText( "" );
  d->DeleteVesselsLineEdit->setText( "" );
  d->ApplyPushButton->setEnabled( d->inputSpatialObject != 0 );
  d->Table->restoreDefaults();
}