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

#ifndef __qSlicerInteractiveTubesToTreeModuleWidget_h
#define __qSlicerInteractiveTubesToTreeModuleWidget_h


// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerInteractiveTubesToTreeModuleExport.h"

class qSlicerInteractiveTubesToTreeModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLSpatialObjectsNode;
class vtkSlicerInteractiveTubesToTreeLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_INTERACTIVETUBESTOTREE_EXPORT qSlicerInteractiveTubesToTreeModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerInteractiveTubesToTreeModuleWidget(QWidget *parent=0);
  virtual ~qSlicerInteractiveTubesToTreeModuleWidget();

public slots:
  void setInputSpatialObjectsNode(vtkMRMLNode* node);
  void setInputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node);
  void setOutputSpatialObjectsNode(vtkMRMLNode* node);
  void setOutputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node);
  void restoreDefaults();
  void runConversion();

protected:
  QScopedPointer<qSlicerInteractiveTubesToTreeModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerInteractiveTubesToTreeModuleWidget);
  Q_DISABLE_COPY(qSlicerInteractiveTubesToTreeModuleWidget);
};

#endif
