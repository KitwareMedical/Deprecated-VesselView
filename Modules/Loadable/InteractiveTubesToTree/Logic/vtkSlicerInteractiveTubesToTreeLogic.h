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

#ifndef __vtkSlicerInteractiveTubesToTreeLogic_h
#define __vtkSlicerInteractiveTubesToTreeLogic_h

class vtkColorTransferFunction;
class vtkMRMLMarkupsNode;
class vtkMRMLSpatialObjectsDisplayNode;
class vtkMRMLVolumeNode;
class vtkSlicerCLIModuleLogic;
class vtkSlicerSpatialObjectsLogic;

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// ITK includes
#include "itkVector.h"
#include "itkVesselTubeSpatialObject.h"

// STD includes
#include <cstdlib>
#include <map>

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"
#include "vtkSlicerInteractiveTubesToTreeModuleLogicExport.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_INTERACTIVETUBESTOTREE_MODULE_LOGIC_EXPORT
  vtkSlicerInteractiveTubesToTreeLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerInteractiveTubesToTreeLogic *New();
  vtkTypeMacro( vtkSlicerInteractiveTubesToTreeLogic, vtkSlicerModuleLogic );
  void PrintSelf( ostream& os, vtkIndent indent );
  void SetConversionLogic( vtkSlicerCLIModuleLogic* logic );
  vtkSlicerCLIModuleLogic* GetConversionLogic();
  void SetSpatialObjectsLogic( vtkSlicerSpatialObjectsLogic* logic );
  vtkSlicerSpatialObjectsLogic* GetSpatialObjectsLogic();

  // typdefs
  typedef itk::GroupSpatialObject< 3 >                              TubeNetType1;
  typedef TubeNetType1::Pointer                                     TubeNetPointerType;
  typedef vtkMRMLSpatialObjectsNode::TubeNetType                    TubeNetType;
  typedef itk::VesselTubeSpatialObject< 3 >                         VesselTubeType;
  typedef VesselTubeType::TubePointType                             VesselTubePointType;
  typedef itk::Point< double, 3 >                                   PointType;
  typedef std::vector< VesselTubePointType >                        PointListType;
  typedef itk::VesselTubeSpatialObjectPoint< 3 >                    TestType;

  bool Apply( vtkMRMLSpatialObjectsNode* inputNode, vtkMRMLSpatialObjectsNode* outputNode,
    double maxTubeDistanceToRadiusRatio,
    double maxContinuityAngleError,
    bool removeOrphanTubes,
    std::string rootTubeIdList );
  void ConnectTubesInSpatialObject( vtkMRMLSpatialObjectsNode* spatialNode, int, int );
  std::string ConstructTemporaryFileName( const std::string& name );
  void CreateTubeColorColorMap( vtkMRMLSpatialObjectsNode* spatialNode,
    vtkMRMLSpatialObjectsDisplayNode* spatialDisplayNode );
  void deleteTubeFromSpatialObject( vtkMRMLSpatialObjectsNode* spatialNode );
  int FindNearestTube( vtkMRMLSpatialObjectsNode* inputNode, double* );
  std::string GetOutputFileName();
  std::set< int > GetSpatialObjectChildrenData
    ( vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID );
  void GetSpatialObjectData( vtkMRMLSpatialObjectsNode* spatialNode,
    std::vector< int >& TubeIDList, std::vector< int >& ParentIDList,
    std::vector< bool >& IsRootList, std::vector< bool >& IsArteryList,
    std::vector< double >& RedColorList, std::vector< double >& GreenColorList,
    std::vector< double >& BlueColorList );
  bool GetSpatialObjectOrphanStatusData
     ( vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID );
  int GetSpatialObjectNumberOfTubes( vtkMRMLSpatialObjectsNode* spatialNode );
  void setActivePlaceNodeID(vtkMRMLMarkupsNode* node);
  void SetSpatialObjectColor( vtkMRMLSpatialObjectsNode* spatialNode,
    int currTubeID,
    float red,
    float blue,
    float green );
  void SetOutputFileName( std::string name );
  std::string SaveSpatialObjectNode( vtkMRMLSpatialObjectsNode *spatialObjectsNode );

protected:
  vtkSlicerInteractiveTubesToTreeLogic();
  virtual ~vtkSlicerInteractiveTubesToTreeLogic();
  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene.
  //Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:
  vtkSlicerInteractiveTubesToTreeLogic( const vtkSlicerInteractiveTubesToTreeLogic& );
  void operator= ( const vtkSlicerInteractiveTubesToTreeLogic& );
  class vtkInternal;
  vtkInternal* m_Internal;
  std::string  m_OutputFileName;
};

#endif
