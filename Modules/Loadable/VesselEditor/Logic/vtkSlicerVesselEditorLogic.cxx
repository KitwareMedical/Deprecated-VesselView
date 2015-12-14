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

// VesselEditor Logic includes
#include "vtkSlicerVesselEditorLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMRMLSpatialObjectsDisplayNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>

//Spatial Objects includes
#include "vtkSlicerSpatialObjectsLogic.h"

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
class vtkSlicerVesselEditorLogic::vtkInternal
{
public:
  vtkInternal();

  vtkSlicerSpatialObjectsLogic* SpatialObjectsLogic;
};

//----------------------------------------------------------------------------
vtkSlicerVesselEditorLogic::vtkInternal::vtkInternal()
{
  this->SpatialObjectsLogic = 0;
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVesselEditorLogic);

//----------------------------------------------------------------------------
vtkSlicerVesselEditorLogic::vtkSlicerVesselEditorLogic()
{
   this->Internal = new vtkInternal;
}

//----------------------------------------------------------------------------
vtkSlicerVesselEditorLogic::~vtkSlicerVesselEditorLogic()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic::SetSpatialObjectsLogic(vtkSlicerSpatialObjectsLogic* logic)
{
  this->Internal->SpatialObjectsLogic = logic;
}

//----------------------------------------------------------------------------
vtkSlicerSpatialObjectsLogic* vtkSlicerVesselEditorLogic::GetSpatialObjectsLogic()
{
  return this->Internal->SpatialObjectsLogic;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}