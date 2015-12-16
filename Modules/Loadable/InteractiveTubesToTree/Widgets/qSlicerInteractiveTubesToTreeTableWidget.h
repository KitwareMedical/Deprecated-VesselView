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

#ifndef __qSlicerInteractiveTubesToTreeTableWidget_h
#define __qSlicerInteractiveTubesToTreeTableWidget_h

// Qt includes
#include "qSlicerAbstractModuleWidget.h"

// CTK includes
#include <ctkVTKObject.h>

// Table Widgets includes
#include "qSlicerInteractiveTubesToTreeModuleWidgetsExport.h"

class qSlicerInteractiveTubesToTreeTableWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLSpatialObjectsNode;
class vtkMRMLSpatialObjectsDisplayNode;
class vtkMRMLSpatialObjectsDisplayPropertiesNode;
class QTableWidgetItem;
class vtkLookupTable;
class vtkMRMLScene;
class vtkColorTransferFunction;

/// \ingroup Slicer_QtModules_InteractiveTubesToTree
class Q_SLICER_MODULE_INTERACTIVETUBESTOTREE_WIDGETS_EXPORT qSlicerInteractiveTubesToTreeTableWidget
  : public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerInteractiveTubesToTreeTableWidget(QWidget *parent=0);
  virtual ~qSlicerInteractiveTubesToTreeTableWidget();

  bool qSlicerInteractiveTubesToTreeTableWidget::getTubeDisplayColor(QColor&, int row);
  vtkMRMLSpatialObjectsDisplayNode* SpatialObjectsDisplayNode() const;
  void buildTubeDisplayTable();
  std::string getSelectedRootIds();
  void selectRow(int rowID, bool isDefault);
  bool isRowSelected(int rowID, int tubeID);

public slots:
  /// Set the MRML node of interest
  void setSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node);
  void setSpatialObjectsNode(vtkMRMLNode* node);
  void onTableCellClicked(QTableWidgetItem* item);
  void onCurTubeColorChanged(const QColor&);
  void onRowTubeColorChanged(const QColor &color, int rowID);
  void onClickHorizontalHeader(int column);
  void onClickMarkSelectedAsRoot();
  void setSpatialObjectsDisplayNodeMode();

protected slots:
  void updateWidgetFromMRML();
  //void updateMRMLFromWidget();
  void setSpatialObjectsDisplayNode(vtkMRMLNode *node);
  void setSpatialObjectsDisplayNode(vtkMRMLSpatialObjectsDisplayNode *node);
  void onTableSelectionChanged();

protected:
  QScopedPointer<qSlicerInteractiveTubesToTreeTableWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerInteractiveTubesToTreeTableWidget);
  Q_DISABLE_COPY(qSlicerInteractiveTubesToTreeTableWidget);
  QColor defaultColor;
};

#endif
