/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// .NAME vtkSlicerInteractiveTubesToTreeLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerInteractiveTubesToTreeLogic_h
#define __vtkSlicerInteractiveTubesToTreeLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
class vtkSlicerCLIModuleLogic;
class vtkSlicerSpatialObjectsLogic;
class vtkMRMLSpatialObjectsDisplayNode;
class vtkMRMLVolumeNode;
class vtkColorTransferFunction;
class vtkMRMLMarkupsNode;

// ITK includes
#include "itkVesselTubeSpatialObject.h"
#include "itkVector.h"

// MRML includes

// STD includes
#include <cstdlib>

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"

#include "vtkSlicerInteractiveTubesToTreeModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_INTERACTIVETUBESTOTREE_MODULE_LOGIC_EXPORT vtkSlicerInteractiveTubesToTreeLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerInteractiveTubesToTreeLogic *New();
  vtkTypeMacro(vtkSlicerInteractiveTubesToTreeLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetConversionLogic(vtkSlicerCLIModuleLogic* logic);
  vtkSlicerCLIModuleLogic* GetConversionLogic();
  void SetSpatialObjectsLogic(vtkSlicerSpatialObjectsLogic* logic);
  vtkSlicerSpatialObjectsLogic* GetSpatialObjectsLogic();

  // typdefs
  typedef itk::GroupSpatialObject<3>                                TubeNetType1;
  typedef TubeNetType1::Pointer                                     TubeNetPointerType;
  typedef vtkMRMLSpatialObjectsNode::TubeNetType                    TubeNetType;
  typedef itk::VesselTubeSpatialObject<3>                           VesselTubeType;
  typedef VesselTubeType::TubePointType                             VesselTubePointType;
  typedef itk::Point<double, 3>                                     PointType;


  bool Apply(vtkMRMLSpatialObjectsNode* inputNode, vtkMRMLSpatialObjectsNode* outputNode, double maxTubeDistanceToRadiusRatio,
    double maxContinuityAngleError, bool removeOrphanTubes, std::string rootTubeIdList);
  int FindNearestTube(vtkMRMLSpatialObjectsNode* inputNode, double*);
  void setActivePlaceNodeID(vtkMRMLMarkupsNode* node);
  std::string ConstructTemporaryFileName(const std::string& name);
  std::string SaveSpatialObjectNode(vtkMRMLSpatialObjectsNode *spatialObjectsNode);
  std::string GetOutputFileName();
  void SetOutputFileName(std::string name);
  void GetSpatialObjectData(vtkMRMLSpatialObjectsNode* spatialNode, std::vector<int>& TubeIDList);
  void SetSpatialObjectColorData(vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID, float red, float blue, float green);
  void deleteTubeFromSpatialObject(vtkMRMLSpatialObjectsNode* spatialNode, std::set<int> tubeIDs);
  void CreateTubeColorColorMap(vtkMRMLSpatialObjectsNode* spatialNode, vtkMRMLSpatialObjectsDisplayNode* spatialDisplayNode);
  bool GetSpatialObjectRootStatusData(vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID);
  int GetSpatialObjectParentIdData(vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID);
  bool GetSpatialObjectOrphanStatusData(vtkMRMLSpatialObjectsNode* spatialNode, int currTubeID);

protected:
  vtkSlicerInteractiveTubesToTreeLogic();
  virtual ~vtkSlicerInteractiveTubesToTreeLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:

  vtkSlicerInteractiveTubesToTreeLogic(const vtkSlicerInteractiveTubesToTreeLogic&); // Not implemented
  void operator=(const vtkSlicerInteractiveTubesToTreeLogic&); // Not implemented
  class vtkInternal;
  vtkInternal* Internal;
  std::string OutputFileName;
};

#endif
