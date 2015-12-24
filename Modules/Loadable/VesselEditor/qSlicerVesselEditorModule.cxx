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
#include <QtPlugin>

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

// VesselEditor Logic includes
#include <vtkSlicerVesselEditorLogic.h>
#include "vtkSlicerSpatialObjectsLogic.h"

// VesselEditor includes
#include "qSlicerVesselEditorModule.h"
#include "qSlicerVesselEditorModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVesselEditorModule, qSlicerVesselEditorModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVesselEditorModulePrivate
{
public:
  qSlicerVesselEditorModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModulePrivate::qSlicerVesselEditorModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModule methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModule::qSlicerVesselEditorModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVesselEditorModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVesselEditorModule::~qSlicerVesselEditorModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVesselEditorModule::helpText() const
{
  return "This module can be used to make some basic editions to a vessel"
    "tree network like deletion and merging.";
}

//-----------------------------------------------------------------------------
QString qSlicerVesselEditorModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVesselEditorModule::icon() const
{
  return QIcon(":/Icons/VesselEditor.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::categories() const
{
  return QStringList() << "TubeTK";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::dependencies() const
{
  QStringList moduleDependencies;
  moduleDependencies << "SpatialObjects";
  moduleDependencies << "InteractiveTubesToTree";
  return moduleDependencies;
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVesselEditorModule::setup()
{
  this->Superclass::setup();
   vtkSlicerVesselEditorLogic* vesselEditorLogic =
	  vtkSlicerVesselEditorLogic::SafeDownCast(this->logic());

   qSlicerAbstractCoreModule* spatialObjectsModule =
    qSlicerCoreApplication::application()->moduleManager()->module("SpatialObjects");

   if (spatialObjectsModule)
    {
      vtkSlicerSpatialObjectsLogic* spatialObjectsLogic =
        vtkSlicerSpatialObjectsLogic::SafeDownCast(spatialObjectsModule->logic());
      vesselEditorLogic->SetSpatialObjectsLogic(spatialObjectsLogic);
    }

}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerVesselEditorModule
::createWidgetRepresentation()
{
  return new qSlicerVesselEditorModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVesselEditorModule::createLogic()
{
  return vtkSlicerVesselEditorLogic::New();
}
