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

//std include
#include <map>
#include <set>

// CTK includes
#include <ctkVTKObject.h>

// Table Widgets includes
#include "qSlicerInteractiveTubesToTreeModuleWidgetsExport.h"

class qSlicerInteractiveTubesToTreeTableWidgetPrivate;
class QTableWidgetItem;

class vtkColorTransferFunction;
class vtkLookupTable;
class vtkMRMLMarkupsNode;
class vtkMRMLNode;
class vtkMRMLScene;
class vtkMRMLSpatialObjectsDisplayNode;
class vtkMRMLSpatialObjectsDisplayPropertiesNode;
class vtkMRMLSpatialObjectsNode;

/// \ingroup Slicer_QtModules_InteractiveTubesToTree
class Q_SLICER_MODULE_INTERACTIVETUBESTOTREE_WIDGETS_EXPORT
  qSlicerInteractiveTubesToTreeTableWidget
  : public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerInteractiveTubesToTreeTableWidget( QWidget *parent = 0 );
  virtual ~qSlicerInteractiveTubesToTreeTableWidget();

  vtkMRMLSpatialObjectsDisplayNode* SpatialObjectsDisplayNode() const;
  vtkMRMLMarkupsNode* getMRMLMarkupsNode();

  void ChangeTubeColor( const QColor &color, int tubeID, int rowID = -1 );
  bool ChangeSpatialObjectColorMap( const QColor &color, int tubeID );
  int getColumnIndex( std::string columnName );
  int getRowIDfromTubeID( int tubeID );
  int getTubeIDfromRowID( int rowID );
  void hideColumn( int colID );
  bool isRowSelected( int rowID, int tubeID = -1 );
  void SelectTube( int tubeID, int rowID = -1 );
  void unSelectTube( int tubeID, int rowID = -1 );
  void buildTubeDisplayTable();

public slots:
  /// Set the MRML node of interest

  void findTubeIDs( int n );
  void onActionColorTree();
  void onClickApplyColor();
  void onClickDeleteSelected();
  void onClickHorizontalHeader( int column );
  void onClickRefreshTable();
  void onClickSelectAllOrphans();
  void onClickSelectAllRoots();
  void onClickShowHideTubes( bool );
  void onClickTubeColorPicker( const QColor& );
  void onMarkupAddEvent();
  void onNodeAddedEvent( vtkObject*, vtkObject* node );
  void onNthMarkupModifiedEvent( vtkObject *caller, vtkObject *callData );
  void onPressedShowOrphans();
  void onPressedShowRoots();
  void onReleasedShowOrphans();
  void onReleasedShowRoots();
  void onSelectTubeColorChanged( const QColor& );
  void onShowRootsColorChanged( const QColor& );
  void onShowOrphansColorChanged( const QColor& );
  void onTableCellClicked( QTableWidgetItem* item );
  void restoreDefaults();
  void setSpatialObjectsDisplayNodeMode();
  void setSpatialObjectsNode( vtkMRMLSpatialObjectsNode* node );
  void setSpatialObjectsNode( vtkMRMLNode* node );

protected slots:
  void onTableSelectionChanged();
  void setSpatialObjectsDisplayNode( vtkMRMLNode *node );
  void setSpatialObjectsDisplayNode( vtkMRMLSpatialObjectsDisplayNode *node );
  void updateMRMLFromWidget();
  void updateWidgetFromMRML();

protected:
  QScopedPointer< qSlicerInteractiveTubesToTreeTableWidgetPrivate > d_ptr;

private:
  Q_DECLARE_PRIVATE( qSlicerInteractiveTubesToTreeTableWidget );
  Q_DISABLE_COPY( qSlicerInteractiveTubesToTreeTableWidget );
};

#endif
