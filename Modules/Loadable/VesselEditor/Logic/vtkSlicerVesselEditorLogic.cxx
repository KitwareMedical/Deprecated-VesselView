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

//---------------------------------------------------------------------------=
void vtkSlicerVesselEditorLogic
::ConnectTubesInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int sourceTubeID, int targetTubeID)
{
  if (!spatialNode)
  {
    return ;
  }

  TubeNetType* spatialObject = spatialNode->GetSpatialObject();
  VesselTubePointType* targetStartPoint;
  VesselTubePointType* targetEndPoint;
  VesselTubeType* targetTube;
  double minDistance = INT32_MAX;
  VesselTubePointType* sourceNearestPoint;

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube || currTube->GetNumberOfPoints() < 1)
    {
      continue;
    }
    if (currTube->GetId() == targetTubeID)
    {
      targetTube = currTube;
      int numberOfPoints = currTube->GetNumberOfPoints();
      targetStartPoint =
        dynamic_cast<VesselTubePointType*>(currTube->GetPoint(0));
      targetEndPoint =
        dynamic_cast<VesselTubePointType*>(currTube->GetPoint(numberOfPoints-1));
     break; 
    }
  }
  int flag = -1;
  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube || currTube->GetNumberOfPoints() < 1)
    {
      continue;
    }    
    if (currTube->GetId() == sourceTubeID)
    {
      int numberOfPoints = currTube->GetNumberOfPoints();
      for (int index = 0; index < numberOfPoints; index++)
      {
        VesselTubePointType* tubePoint =
          dynamic_cast<VesselTubePointType*>(currTube->GetPoint(index));
        PointType sourcePointPosition = tubePoint->GetPosition();
        double distance = sourcePointPosition.SquaredEuclideanDistanceTo(targetStartPoint->GetPosition());
        if (minDistance > distance)
        {
          minDistance = distance;
          sourceNearestPoint = tubePoint;
          flag = 1;
        }
        distance = sourcePointPosition.SquaredEuclideanDistanceTo(targetEndPoint->GetPosition());
        if (minDistance > distance)
        {
          minDistance = distance;
          sourceNearestPoint = tubePoint;
          flag = 2;
        }
      }
     break; 
    }
  }
  if(flag == 1)//add as the starting point
  {
    PointListType targetTubePoints = targetTube->GetPoints();
    targetTube->Clear();
    targetTube->GetPoints().push_back(*sourceNearestPoint);
    for(int i=0;i<targetTubePoints.size();i++)
    {
      targetTube->GetPoints().push_back(targetTubePoints[i]);
    }
  }
  if(flag == 2)// add as an end point
  {
    targetTube->GetPoints().push_back(*sourceNearestPoint);
  }
  spatialNode->UpdatePolyDataFromSpatialObject();
  return ;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic
::DisconnectTubesInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int sourceTubeID, int targetTubeID)
{
  if (!spatialNode)
  {
    return ;
  }

  TubeNetType* spatialObject = spatialNode->GetSpatialObject();
  VesselTubePointType* targetStartPoint;
  VesselTubePointType* targetEndPoint;
  VesselTubeType* targetTube;
  double minDistance = INT32_MAX;
  VesselTubePointType* sourceNearestPoint;

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube || currTube->GetNumberOfPoints() < 1)
    {
      continue;
    }
    if (currTube->GetId() == targetTubeID)
    {
      targetTube = currTube;
      int numberOfPoints = currTube->GetNumberOfPoints();
      targetStartPoint =
        dynamic_cast<VesselTubePointType*>(currTube->GetPoint(0));
      targetEndPoint =
        dynamic_cast<VesselTubePointType*>(currTube->GetPoint(numberOfPoints-1));
     break; 
    }
  }
  int flag = -1;
  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube || currTube->GetNumberOfPoints() < 1)
    {
      continue;
    }    
    if (currTube->GetId() == sourceTubeID)
    {
      int numberOfPoints = currTube->GetNumberOfPoints();
      for (int index = 0; index < numberOfPoints; index++)
      {
        VesselTubePointType* tubePoint =
          dynamic_cast<VesselTubePointType*>(currTube->GetPoint(index));
        PointType sourcePointPosition = tubePoint->GetPosition();
        double distance = sourcePointPosition.SquaredEuclideanDistanceTo(targetStartPoint->GetPosition());
        if (minDistance > distance)
        {
          minDistance = distance;
          sourceNearestPoint = tubePoint;
          flag = 1;
        }
        distance = sourcePointPosition.SquaredEuclideanDistanceTo(targetEndPoint->GetPosition());
        if (minDistance > distance)
        {
          minDistance = distance;
          sourceNearestPoint = tubePoint;
          flag = 2;
        }
      }
     break; 
    }
  }
  if(minDistance < 5)
  {
    if(flag == 1)//add as the starting point
    {
      PointListType targetTubePoints = targetTube->GetPoints();
      targetTube->Clear();
      for(int i=1;i<targetTubePoints.size();i++)
      {
        targetTube->GetPoints().push_back(targetTubePoints[i]);
      }
    }
    if(flag == 2)// add as an end point
    {
      targetTube->GetPoints().pop_back();
    }
    spatialNode->UpdatePolyDataFromSpatialObject();
  }
  return ;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselEditorLogic
::SplitTubeInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int tubeID, double *xyz)
{
  if (!spatialNode)
  {
    return ;
  }
  PointType splittingPoint = PointType(xyz);
  TubeNetType* spatialObject = spatialNode->GetSpatialObject();
  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  for (TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
  {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube || currTube->GetNumberOfPoints() < 1)
    {
      continue;
    }
    if (currTube->GetId() == tubeID)
    {
      int numberOfPoints = currTube->GetNumberOfPoints();
      double minDistance = INT32_MAX;
      int nearestPoint = -1;
      double meanRadius = 0;
      for (int index = 0; index < numberOfPoints; index++)
      {
        VesselTubePointType* tubePoint =
          dynamic_cast<VesselTubePointType*>(currTube->GetPoint(index));
        PointType tubePointPosition = tubePoint->GetPosition();
        tubePointPosition =
        currTube->GetIndexToWorldTransform()->TransformPoint(tubePointPosition);
        double distance = tubePointPosition.EuclideanDistanceTo(splittingPoint);
        if( distance < minDistance)
        {
          minDistance = distance;
          nearestPoint = index;
        }
        meanRadius += tubePoint->GetRadius();
      }
      meanRadius = meanRadius/numberOfPoints;
      if(minDistance < meanRadius)
      {
        VesselTubeType::Pointer newTube = VesselTubeType::New();
        newTube->CopyInformation(currTube);
        // TODO: make CopyInformation of itk::SpatialObject do this
        newTube->GetObjectToParentTransform()->SetScale(
        currTube->GetObjectToParentTransform()->GetScale() );
        newTube->GetObjectToParentTransform()->SetOffset(
          currTube->GetObjectToParentTransform()->GetOffset() );
        newTube->GetObjectToParentTransform()->SetMatrix(
          currTube->GetObjectToParentTransform()->GetMatrix() );
        newTube->SetSpacing( currTube->GetSpacing() );
        newTube->ComputeObjectToWorldTransform();
        newTube->ComputeTangentAndNormals();
        newTube->SetRoot( false );
        newTube->SetParentId(currTube->GetId());
        newTube->Clear();
        for (int index = nearestPoint; index < numberOfPoints; index++)
        {
          VesselTubePointType* tubePoint =
            dynamic_cast<VesselTubePointType*>(currTube->GetPoint(index));
          newTube->GetPoints().push_back(*tubePoint);
        }
        for (int index = numberOfPoints-1; index > nearestPoint; index--)
        {
          currTube->GetPoints().pop_back();
        }
        currTube->AddSpatialObject(newTube);
      }
      spatialNode->UpdatePolyDataFromSpatialObject();
    }
  }
}