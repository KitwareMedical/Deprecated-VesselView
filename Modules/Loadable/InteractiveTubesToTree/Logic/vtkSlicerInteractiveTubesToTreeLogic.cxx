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
#include <QDebug>

// CLI invocation
#include <qSlicerCLIModule.h>
#include <qSlicerApplication.h>
#include <vtkSlicerCLIModuleLogic.h>

// InteractiveTubesToTree Logic includes
#include "vtkSlicerInteractiveTubesToTreeLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include "vtkMRMLVolumeNode.h"
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLMarkupsNode.h>
//Spatial Objects includes
#include "vtkSlicerSpatialObjectsLogic.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkColorTransferFunction.h>
#include <vtkMRMLSpatialObjectsDisplayNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
struct DigitsToCharacters
{
  char operator() (char in)
  {
    if (in >= 48 && in <= 57)
    {
      return in + 17;
    }

    return in;
  }
};

//----------------------------------------------------------------------------
class vtkSlicerInteractiveTubesToTreeLogic::vtkInternal
{
public:
  vtkInternal();

  vtkSlicerCLIModuleLogic* ConversionLogic;
  vtkSlicerSpatialObjectsLogic* SpatialObjectsLogic;
};

//----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic::vtkInternal::vtkInternal()
{
  this->ConversionLogic = 0;
  this->SpatialObjectsLogic = 0;
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerInteractiveTubesToTreeLogic);

//----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic::vtkSlicerInteractiveTubesToTreeLogic()
{
  this->Internal = new vtkInternal;
}

//----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic::~vtkSlicerInteractiveTubesToTreeLogic()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::SetConversionLogic(vtkSlicerCLIModuleLogic* logic)
{
  this->Internal->ConversionLogic = logic;
}

//----------------------------------------------------------------------------
vtkSlicerCLIModuleLogic* vtkSlicerInteractiveTubesToTreeLogic::GetConversionLogic()
{
  return this->Internal->ConversionLogic;
}

//----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::SetSpatialObjectsLogic(vtkSlicerSpatialObjectsLogic* logic)
{
  this->Internal->SpatialObjectsLogic = logic;
}

//----------------------------------------------------------------------------
vtkSlicerSpatialObjectsLogic* vtkSlicerInteractiveTubesToTreeLogic::GetSpatialObjectsLogic()
{
  return this->Internal->SpatialObjectsLogic;
}

//----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::SetOutputFileName(std::string name)
{
  this->OutputFileName = name;
  return;
}

//----------------------------------------------------------------------------
std::string vtkSlicerInteractiveTubesToTreeLogic::GetOutputFileName()
{
  qCritical(this->OutputFileName.c_str());
  return this->OutputFileName;
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
bool vtkSlicerInteractiveTubesToTreeLogic
::Apply(vtkMRMLSpatialObjectsNode* inputNode, vtkMRMLSpatialObjectsNode* outputNode, double maxTubeDistanceToRadiusRatio,
double maxContinuityAngleError, bool removeOrphanTubes, std::string rootTubeIdList)
{
  qCritical("In logic!!");

  if (!inputNode || !outputNode)
  {
    return false;
  }
  if (this->Internal->ConversionLogic == 0)
  {
    qCritical("InteractiveTubesToTreeLogic: ERROR: conversion logic is not set!");
    return false;
  }
  if (!this->Internal->SpatialObjectsLogic)
  {
    std::cerr << "InteractiveTubesToTreeLogic: ERROR: failed to get hold of Spatial Objects logic" << std::endl;
    return -2;
  }
  vtkSmartPointer<vtkMRMLCommandLineModuleNode> cmdNode = this->Internal->ConversionLogic->CreateNodeInScene();
  if (cmdNode.GetPointer() == 0)
  {
    qCritical("In logic!! Command Line Module Node error");
    return false;
  }
  std::string outputfileName = ConstructTemporaryFileName(outputNode->GetID());
  this->SetOutputFileName(outputNode->GetName());
  cmdNode->SetParameterAsString("inputTREFile", SaveSpatialObjectNode(inputNode));
  cmdNode->SetParameterAsString("outputTREFile", outputfileName);
  cmdNode->SetParameterAsDouble("maxTubeDistanceToRadiusRatio", maxTubeDistanceToRadiusRatio);
  cmdNode->SetParameterAsDouble("maxContinuityAngleError", maxContinuityAngleError);
  cmdNode->SetParameterAsString("rootTubeIdList", rootTubeIdList);
  this->Internal->ConversionLogic->ApplyAndWait(cmdNode);

  this->Internal->SpatialObjectsLogic->SetSpatialObject(outputNode, outputfileName.c_str());
  outputNode->SetName(this->GetOutputFileName().c_str());

  this->GetMRMLScene()->RemoveNode(cmdNode);

  qCritical(ConstructTemporaryFileName(inputNode->GetID()).c_str());
  return true;
}

//----------------------------------------------------------------------------
int vtkSlicerInteractiveTubesToTreeLogic::FindNearestTube(vtkMRMLSpatialObjectsNode* spatialNode, double *xyz)
{
  if (!spatialNode)
  {
    return -1;
  }

  PointType fiducial = PointType(xyz);
  double minDistance = INT32_MAX;
  int finalTubeID = -1;

  TubeNetType* spatialObject = spatialNode->GetSpatialObject();

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube)
    {
      continue;
    }

    int numberOfPoints = currTube->GetNumberOfPoints();
    for (int index = 0; index < numberOfPoints; index++)
    {
      VesselTubePointType* tubePoint =
        dynamic_cast<VesselTubePointType*>(currTube->GetPoint(index));

      PointType inputPoint = tubePoint->GetPosition();

      inputPoint =
        currTube->GetIndexToWorldTransform()->TransformPoint(inputPoint);

      double distance = inputPoint.SquaredEuclideanDistanceTo(fiducial);
      int i = 0;
      if (minDistance > distance)
      {
        minDistance = distance;
        finalTubeID = currTube->GetId();
      }
    }
  }
  return finalTubeID;
}

//----------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic::setActivePlaceNodeID(vtkMRMLMarkupsNode* node)
{
  if (node)
  {
    qSlicerApplication * app = qSlicerApplication::application();
    vtkMRMLSelectionNode* selectionNode = app->applicationLogic()->GetSelectionNode();
    selectionNode->SetActivePlaceNodeID(node->GetID());
  }
  return;
}
//----------------------------------------------------------------------------
std::string vtkSlicerInteractiveTubesToTreeLogic
::SaveSpatialObjectNode(vtkMRMLSpatialObjectsNode *spatialObjectsNode)
{
  std::string filename = ConstructTemporaryFileName(spatialObjectsNode->GetID());
  this->Internal->SpatialObjectsLogic->SaveSpatialObject(filename.c_str(), spatialObjectsNode);
  return filename;
}

//----------------------------------------------------------------------------
std::string vtkSlicerInteractiveTubesToTreeLogic
::ConstructTemporaryFileName(const std::string& name)
{
  std::string fname = name;
  std::string pid;
  std::ostringstream pidString;

  // Encode process id into a string.  To avoid confusing the
  // Archetype reader, convert the numbers in pid to characters [0-9]->[A-J]
#ifdef _WIN32
  pidString << GetCurrentProcessId();
#else
  pidString << getpid();
#endif
  pid = pidString.str();
  std::transform(pid.begin(), pid.end(), pid.begin(), DigitsToCharacters());

  // To avoid confusing the Archetype readers, convert any
  // numbers in the filename to characters [0-9]->[A-J]
  std::transform(fname.begin(), fname.end(), fname.begin(), DigitsToCharacters());

  // By default, the filename is based on the temporary directory and the pid
  std::string temporaryDirectory = ".";
  vtkSlicerApplicationLogic* appLogic = this->GetApplicationLogic();
  if (appLogic)
  {
    temporaryDirectory = appLogic->GetTemporaryPath();
  }
  fname = temporaryDirectory + "/" + pid + "_" + fname + ".tre";
  return fname;
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic
::GetSpatialObjectData(vtkMRMLSpatialObjectsNode* spatialNode, std::vector<int>& TubeIDList)
{
  if (!spatialNode)
  {
    return ;
  }
  TubeNetType* spatialObject = spatialNode->GetSpatialObject();

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube)
    {
      continue;
    }
    if (currTube->GetNumberOfPoints() < 2)
    {
      std::cerr << "Error, vessel #" << currTube->GetId()
        << " has less than 2 points !" << std::endl;
    }
    int currID = currTube->GetId();
    TubeIDList.push_back(currID);
  }
  return ;
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic
::SetSpatialObjectData(vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID, float red, float green, float blue)
{
  if (!spatialNode)
  {
    return;
  }
  TubeNetType* spatialObject = spatialNode->GetSpatialObject();

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube)
    {
      continue;
    }
    if (currTube->GetId() == currTubeID)
    {
      currTube->GetProperty()->SetColor(red, green, blue);
      break;
    }
    else
    {
      continue;
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerInteractiveTubesToTreeLogic
::CreateTubeColorColorMap(vtkMRMLSpatialObjectsNode* spatialNode,  vtkMRMLSpatialObjectsDisplayNode* spatialDisplayNode)
{
  if (!spatialNode)
  {
    return;
  }
  char colorMapName[80];
  strcpy(colorMapName, spatialNode->GetName());
  strcat(colorMapName, "_TubeColor");
  vtkMRMLNode* colorNode1 = spatialDisplayNode->GetScene()->GetFirstNodeByName(colorMapName);
  vtkMRMLProceduralColorNode* colorNode = vtkMRMLProceduralColorNode::SafeDownCast(colorNode1);

  if (colorNode == NULL)
  {
    vtkNew<vtkMRMLProceduralColorNode> colorNode;
    colorNode->SetName(colorMapName);
    colorNode->SetAttribute("Category", "Tube Color Display");
    colorNode->SetHideFromEditors(false);

    vtkColorTransferFunction* colorMap = colorNode->GetColorTransferFunction();
    colorMap->SetIndexedLookup(0);
    TubeNetType* spatialObject = spatialNode->GetSpatialObject();

    char childName[] = "Tube";
    TubeNetType::ChildrenListType* tubeList =
      spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

    for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
    {
      VesselTubeType* currTube =
        dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
      if (!currTube)
      {
        continue;
      }
      if (currTube->GetNumberOfPoints() < 2)
      {
        std::cerr << "Error, vessel #" << currTube->GetId()
          << " has less than 2 points !" << std::endl;
      }

      colorMap->AddRGBPoint(currTube->GetId(), currTube->GetProperty()->GetColor().GetRed(), currTube->GetProperty()->GetColor().GetGreen(), currTube->GetProperty()->GetColor().GetBlue());
    }

    spatialDisplayNode->GetScene()->AddNode(colorNode.GetPointer());
    spatialDisplayNode->SetAndObserveColorNodeID(colorNode->GetID());
  }
  else
  {
    vtkColorTransferFunction* colorMap = colorNode->GetColorTransferFunction();
    colorMap->SetIndexedLookup(0);
    TubeNetType* spatialObject = spatialNode->GetSpatialObject();

    char childName[] = "Tube";
    TubeNetType::ChildrenListType* tubeList =
      spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

    if (colorMap->GetSize() != tubeList->size())
    {
      colorMap->RemoveAllPoints();
      for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
      {
        VesselTubeType* currTube =
          dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
        if (!currTube)
        {
          continue;
        }
        if (currTube->GetNumberOfPoints() < 2)
        {
          std::cerr << "Error, vessel #" << currTube->GetId()
            << " has less than 2 points !" << std::endl;
        }

        colorMap->AddRGBPoint(currTube->GetId(), currTube->GetProperty()->GetColor().GetRed(), currTube->GetProperty()->GetColor().GetGreen(), currTube->GetProperty()->GetColor().GetBlue());
      }
      spatialDisplayNode->SetAndObserveColorNodeID(colorNode->GetID());
    }
    else
    {
      spatialDisplayNode->SetAndObserveColorNodeID(colorNode->GetID());
    }
  }
}
