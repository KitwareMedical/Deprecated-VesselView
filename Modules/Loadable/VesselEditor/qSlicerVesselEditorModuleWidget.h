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

#ifndef __qSlicerVesselEditorModuleWidget_h
#define __qSlicerVesselEditorModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerVesselEditorModuleExport.h"

class qSlicerVesselEditorModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLSpatialObjectsNode;
class qSlicerInteractiveTubesToTreeTableWidget;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_VESSELEDITOR_EXPORT qSlicerVesselEditorModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVesselEditorModuleWidget(QWidget *parent=0);
  virtual ~qSlicerVesselEditorModuleWidget();
  virtual void enter();

public slots:
  void setInputSpatialObjectsNode(vtkMRMLNode* node);
  void setInputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node);
  void restoreDefaults();
  void onApplySplitTube();
  void onApplyConnectTubes();
  void onApplyDisconnectTubes();
  void onClickConnectSourceTube();
  void onClickConnectTargetTube();
  void onClickDisconnectSourceTube();
  void onClickDisconnectTargetTube();
  void onClickSelectTubeToSplit();

protected:
  QScopedPointer<qSlicerVesselEditorModuleWidgetPrivate> d_ptr;

  virtual void setup();
  void onEnter();

private:
  Q_DECLARE_PRIVATE(qSlicerVesselEditorModuleWidget);
  Q_DISABLE_COPY(qSlicerVesselEditorModuleWidget);
};

#endif
