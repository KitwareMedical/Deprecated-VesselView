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

// .NAME vtkSlicerVesselEditorLogic - slicer logic class
// .SECTION Description


#ifndef __vtkSlicerVesselEditorLogic_h
#define __vtkSlicerVesselEditorLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
class vtkSlicerSpatialObjectsLogic;
class vtkMRMLSpatialObjectsDisplayNode;

// ITK includes
#include "itkVesselTubeSpatialObject.h"
#include "itkVector.h"

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"

// STD includes
#include <cstdlib>

#include "vtkSlicerVesselEditorModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_VESSELEDITOR_MODULE_LOGIC_EXPORT vtkSlicerVesselEditorLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerVesselEditorLogic *New();
  vtkTypeMacro(vtkSlicerVesselEditorLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetSpatialObjectsLogic(vtkSlicerSpatialObjectsLogic* logic);
  vtkSlicerSpatialObjectsLogic* GetSpatialObjectsLogic();

  typedef vtkMRMLSpatialObjectsNode::TubeNetType                    TubeNetType;
  typedef itk::VesselTubeSpatialObject<3>                           VesselTubeType;
  typedef VesselTubeType::TubePointType                             VesselTubePointType;
  typedef itk::Point<double, 3>                                     PointType;
  typedef std::vector< VesselTubePointType >                        PointListType;
  typedef itk::VesselTubeSpatialObjectPoint<3>                      TestType;

  void ConnectTubesInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int parentTubeID, int childTubeID);
  void DisconnectTubesInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int parentTubeID, int childTubeID);
  void SplitTubeInSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, int tubeID, double* splittingPoint);

protected:

  vtkSlicerVesselEditorLogic();
  virtual ~vtkSlicerVesselEditorLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:

  vtkSlicerVesselEditorLogic(const vtkSlicerVesselEditorLogic&); // Not implemented
  void operator=(const vtkSlicerVesselEditorLogic&); // Not implemented
  class vtkInternal;
  vtkInternal* Internal;
};

#endif
