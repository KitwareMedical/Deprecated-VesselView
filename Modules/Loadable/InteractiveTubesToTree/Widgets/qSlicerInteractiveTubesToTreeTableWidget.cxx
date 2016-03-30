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

//Qt imports
#include <QDebug>
#include <QMenuBar>
#include <QSignalMapper>
#include <QStandardItemModel>

//CTK imports
#include <ctkColorPickerButton.h>

// SlicerQt includes
#include "qSlicerApplication.h"

// Table Widgets includes
#include "qSlicerInteractiveTubesToTreeTableWidget.h"
#include "ui_qSlicerInteractiveTubesToTreeTableWidget.h"
#include "vtkSlicerInteractiveTubesToTreeLogic.h"

//vtk includes
#include <vtkColorTransferFunction.h>
#include <vtkDataSet.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>

// MRML includes
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLSpatialObjectsNode.h>
#include <vtkMRMLSpatialObjectsDisplayNode.h>
#include <vtkMRMLSpatialObjectsLineDisplayNode.h>
#include <vtkMRMLSpatialObjectsDisplayPropertiesNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_InteractiveTubesToTree
class qSlicerInteractiveTubesToTreeTableWidgetPrivate
  : public Ui_qSlicerInteractiveTubesToTreeTableWidget
{
  Q_DECLARE_PUBLIC( qSlicerInteractiveTubesToTreeTableWidget );

public:
  qSlicerInteractiveTubesToTreeTableWidgetPrivate(
    qSlicerInteractiveTubesToTreeTableWidget& object );
  vtkSlicerInteractiveTubesToTreeLogic* logic() const;
  virtual void setupUi( qSlicerInteractiveTubesToTreeTableWidget* );

  void init();
  vtkMRMLSpatialObjectsNode* SpatialObjectsNode;
  vtkMRMLSpatialObjectsDisplayNode* SpatialObjectsDisplayNode;
  vtkMRMLMarkupsNode* MarkupsNode;
  int columnIndex( QString label );

protected:
  qSlicerInteractiveTubesToTreeTableWidget* const q_ptr;

private:
  QStringList columnLabels;
};

// --------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidgetPrivate
::qSlicerInteractiveTubesToTreeTableWidgetPrivate(
  qSlicerInteractiveTubesToTreeTableWidget& object )
  : q_ptr( &object )
{
  this->SpatialObjectsNode = 0;
  this->SpatialObjectsDisplayNode = 0;
  this->MarkupsNode = 0;
  this->columnLabels << "Tube ID" << "Color" << "Is Root" << "Is Artery" << "Parent Id";
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidgetPrivate
::setupUi( qSlicerInteractiveTubesToTreeTableWidget* widget )
{
  this->Ui_qSlicerInteractiveTubesToTreeTableWidget::setupUi( widget );
}

//-----------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidgetPrivate::columnIndex( QString label )
{
  return this->columnLabels.indexOf( label );
}

//-----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic*
  qSlicerInteractiveTubesToTreeTableWidgetPrivate::logic() const
{
  Q_Q( const qSlicerInteractiveTubesToTreeTableWidget );
  return vtkSlicerInteractiveTubesToTreeLogic::SafeDownCast( q->logic() );
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeTableWidget methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidget
::qSlicerInteractiveTubesToTreeTableWidget( QWidget* parentWidget )
  : Superclass( parentWidget )
  , d_ptr( new qSlicerInteractiveTubesToTreeTableWidgetPrivate( *this ) )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  d->init();
  // disable as there is not MRML Node associated with the widget
  this->setEnabled( false );
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeTableWidget
::~qSlicerInteractiveTubesToTreeTableWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidgetPrivate::init()
{
  Q_Q(qSlicerInteractiveTubesToTreeTableWidget);

  this->setupUi( q );
  QIcon pushButtonIcon;

  //Select tubes
  this->SelectTubeColorPicker->setColor( QColor( 0, 0, 255 ) );
  this->SelectTubeColorPicker->setDisplayColorName( false );
  QObject::connect( this->SelectTubeColorPicker,
        SIGNAL( colorChanged( QColor ) ), q,
        SLOT( onSelectTubeColorChanged( QColor ) ) );  
  pushButtonIcon.addFile( QString::fromUtf8( ":AnnotationPointWithArrow.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->SelectTubesPushButton->setIcon( pushButtonIcon );
  QObject::connect(
    this->SelectTubesPushButton, SIGNAL( toggled( bool ) ),
    q, SLOT( updateMRMLFromWidget() ) );
  pushButtonIcon.addFile( QString::fromUtf8( ":Refresh.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->RefreshTablePushButton->setIcon( pushButtonIcon );
  QObject::connect(
    this->RefreshTablePushButton, SIGNAL( clicked() ),
    q, SLOT( onClickRefreshTable() ) );

  //Table Widget
  this->TableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
  this->TableWidget->setSelectionMode( QAbstractItemView::MultiSelection );
  this->TableWidget->setColumnCount( this->columnLabels.size() );
  this->TableWidget->setHorizontalHeaderLabels( this->columnLabels );
  this->TableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );

  QObject::connect( this->TableWidget, SIGNAL( itemClicked( QTableWidgetItem* ) ),
    q, SLOT( onTableCellClicked( QTableWidgetItem* ) ) );
  QObject::connect( this->TableWidget, SIGNAL( itemSelectionChanged() ),
    q, SLOT( onTableSelectionChanged() ) );
  QObject::connect( this->TableWidget->horizontalHeader(),
    SIGNAL( sectionDoubleClicked( int ) ), q, SLOT( onClickHorizontalHeader( int ) ) );
  QObject::connect( this->TableWidget->horizontalHeader(),
    SIGNAL( sectionClicked( int ) ), q, SLOT( onClickHorizontalHeader( int ) ) );

  //Apply Color
  pushButtonIcon.addFile( QString::fromUtf8( ":MarkSelected.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->ApplyColorPushButton->setIcon( pushButtonIcon );
  QObject::connect( this->ApplyColorPushButton, SIGNAL( clicked() ),
    q, SLOT( onClickApplyColor() ) );

  //Delete
  pushButtonIcon.addFile( QString::fromUtf8( ":DeleteSelected.png"),
    QSize(), QIcon::Normal, QIcon::Off );
  this->DeleteSelectedPushButton->setIcon( pushButtonIcon );
  QObject::connect( this->DeleteSelectedPushButton, SIGNAL( clicked() ),
    q, SLOT( onClickDeleteSelected() ) );

  //Show/Select Roots
  this->ShowRootsColorPicker->setColor( QColor( 255, 255, 0 ) );
  this->ShowRootsColorPicker->setDisplayColorName( false );
  QObject::connect( this->ShowRootsColorPicker,
        SIGNAL( colorChanged( QColor ) ), q,
        SLOT( onShowRootsColorChanged( QColor ) ) );
  pushButtonIcon.addFile( QString::fromUtf8( ":Highlight.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->ShowRootsPushButton->setIcon( pushButtonIcon );
  this->ShowRootsPushButton->setCheckable( true );
  QObject::connect(
    this->ShowRootsPushButton, SIGNAL( pressed() ),
    q, SLOT( onPressedShowRoots() ) );
  QObject::connect(
    this->ShowRootsPushButton, SIGNAL( released() ),
    q, SLOT( onReleasedShowRoots() ) );
  pushButtonIcon.addFile( QString::fromUtf8( ":Select.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->SelectAllRootsPushButton->setIcon( pushButtonIcon );
  QObject::connect( this->SelectAllRootsPushButton, SIGNAL( clicked() ),
    q, SLOT( onClickSelectAllRoots() ) );

  //Show/Select Orphans
  this->ShowOrphansColorPicker->setColor( QColor( 0, 255, 0 ) );
  this->ShowOrphansColorPicker->setDisplayColorName( false );
  QObject::connect( this->ShowOrphansColorPicker,
        SIGNAL( colorChanged( QColor ) ), q,
        SLOT( onShowOrphansColorChanged( QColor ) ) );
  pushButtonIcon.addFile( QString::fromUtf8( ":Highlight.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->ShowOrphansPushButton->setIcon( pushButtonIcon );
  this->ShowOrphansPushButton->setCheckable( true );
  QObject::connect(
    this->ShowOrphansPushButton, SIGNAL( pressed() ),
    q, SLOT( onPressedShowOrphans() ) );
  QObject::connect(
    this->ShowOrphansPushButton, SIGNAL( released() ),
    q, SLOT( onReleasedShowOrphans() ) );
  pushButtonIcon.addFile( QString::fromUtf8( ":Select.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->SelectAllOrphansPushButton->setIcon( pushButtonIcon );
  QObject::connect( this->SelectAllOrphansPushButton, SIGNAL( clicked() ),
    q, SLOT( onClickSelectAllOrphans() ) );

  //Display Properties
  pushButtonIcon.addFile( QString::fromUtf8( ":Show.png" ),
    QSize(), QIcon::Normal, QIcon::Off );
  this->ShowHidePushButton->setIcon( pushButtonIcon );
  this->ShowHidePushButton->setCheckable( true );
  QObject::connect( this->ShowHidePushButton,
        SIGNAL( toggled( bool ) ),
        q, SLOT( onClickShowHideTubes( bool ) ) );

  // qSlicerApplication * app = qSlicerApplication::application();
  // vtkMRMLInteractionNode* interactionNode =
  //app->applicationLogic()->GetInteractionNode();
  //  q->qvtkReconnect( interactionNode, vtkCommand::ModifiedEvent,
  //   q, SLOT( updateWidgetFromMRML() ) );

  q->setEnabled( this->SpatialObjectsNode != 0 );
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayNode* qSlicerInteractiveTubesToTreeTableWidget::
SpatialObjectsDisplayNode() const
{
  Q_D( const qSlicerInteractiveTubesToTreeTableWidget );

  return d->SpatialObjectsDisplayNode;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget
::setSpatialObjectsNode( vtkMRMLNode* node )
{
  this->setSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast( node ) );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget
::setSpatialObjectsNode( vtkMRMLSpatialObjectsNode* node )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->SpatialObjectsNode == node )
    {
    return;
    }

  vtkMRMLSpatialObjectsNode *oldNode = d->SpatialObjectsNode;
  d->SpatialObjectsNode = node;
  this->setSpatialObjectsDisplayNode(
    d->SpatialObjectsNode ? d->SpatialObjectsNode->GetNthDisplayNode( 1 ) : NULL );

  qvtkReconnect( oldNode, d->SpatialObjectsNode, vtkCommand::ModifiedEvent,
    this, SLOT( updateWidgetFromMRML() ) );

  buildTubeDisplayTable();

//  qSlicerApplication * app = qSlicerApplication::application();
//  vtkMRMLInteractionNode* interactionNode =
//  app->applicationLogic()->GetInteractionNode();
//  interactionNode->SetPlaceModePersistence(1);
  this->updateWidgetFromMRML();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::
setSpatialObjectsDisplayNode( vtkMRMLNode* node )
{
  this->setSpatialObjectsDisplayNode(
    vtkMRMLSpatialObjectsDisplayNode::SafeDownCast( node ) );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::
setSpatialObjectsDisplayNode( vtkMRMLSpatialObjectsDisplayNode*
SpatialObjectsDisplayNode )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->SpatialObjectsDisplayNode == SpatialObjectsDisplayNode )
    {
    return;
    }

  vtkMRMLSpatialObjectsDisplayNode *oldDisplayNode =
    this->SpatialObjectsDisplayNode();
  d->SpatialObjectsDisplayNode = SpatialObjectsDisplayNode;

  qvtkReconnect( oldDisplayNode, this->SpatialObjectsDisplayNode(),
    vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML() ) );

  setSpatialObjectsDisplayNodeMode();// May lead to crash.
  this->updateWidgetFromMRML();
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::setSpatialObjectsDisplayNodeMode()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  d->SpatialObjectsDisplayNode->SetColorModeToScalarData();
  d->SpatialObjectsDisplayNode->SetActiveScalarName( "TubeIDs" );
  d->logic()->CreateTubeColorColorMap
    ( d->SpatialObjectsNode, d->SpatialObjectsDisplayNode );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::updateMRMLFromWidget()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  qSlicerApplication * app = qSlicerApplication::application();
  //  vtkMRMLInteractionNode* interactionNode =
  // app->applicationLogic()->GetInteractionNode();
  // if ( d->SelectTubesPushButton->isChecked() )
  //  {
  //    interactionNode->SetCurrentInteractionMode( interactionNode->Place );
  // }
  // else
  //  {
  //    interactionNode->SetCurrentInteractionMode( interactionNode->ViewTransform );
  //  }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::updateWidgetFromMRML()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  // qSlicerApplication * app = qSlicerApplication::application();
  // vtkMRMLInteractionNode* interactionNode =
  // app->applicationLogic()->GetInteractionNode();
  //  d->SelectTubesPushButton->setChecked
  //  (interactionNode->GetCurrentInteractionMode()== interactionNode->Place);

  if ( !d->SpatialObjectsNode || !d->SpatialObjectsDisplayNode )
    {
    return;
    }
  else
    {
    int NumberOfTubes =
      d->logic()->GetSpatialObjectNumberOfTubes( d->SpatialObjectsNode );
    if ( NumberOfTubes != d->TableWidget->rowCount() )
      {
      setSpatialObjectsDisplayNodeMode();
      this->buildTubeDisplayTable();
      }
    }
}
// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::buildTubeDisplayTable()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->SpatialObjectsNode != 0 )
  {
    this->setEnabled( true );
    //reset the table data
    const std::set< int > selectedTubeIds = d->SpatialObjectsNode->GetSelectedTubeIds();
    std::set< int > copySelectedTubeIds = selectedTubeIds;
    d->SpatialObjectsNode->ClearSelectedTubes();
    d->TableWidget->setRowCount( 0 );

    std::vector< int > TubeIdList;
    std::vector< int > ParentIdList;
    std::vector< bool > IsRootList;
    std::vector< bool > IsArteryList;
    std::vector< double > RedColorList;
    std::vector< double > GreenColorList;
    std::vector< double > BlueColorList;
    d->logic()->GetSpatialObjectData( d->SpatialObjectsNode, TubeIdList, ParentIdList,
      IsRootList, IsArteryList, RedColorList, GreenColorList, BlueColorList );

    if ( TubeIdList.size() == 0 )
      {
      qCritical( "Error while reteriving Spatial Data !" );
      return;
      }

    QIcon pushButtonIcon;
    for ( int i = 0; i < TubeIdList.size(); i++ )
    {
      int newRow = d->TableWidget->rowCount();
      d->TableWidget->insertRow( newRow );

      //Tube ID Column
      int colIndex = d->columnIndex( "Tube ID" );
      QTableWidgetItem* tubeIDItem = new QTableWidgetItem();
      tubeIDItem->setText( QVariant( TubeIdList[i] ).toString() );
      tubeIDItem->setFlags( tubeIDItem->flags() &  ~Qt::ItemIsEditable );
      d->TableWidget->setItem( newRow, colIndex, tubeIDItem );

      //color column
      colIndex = d->columnIndex( "Color" );
      QColor TubeDisplayColor = QColor::fromRgbF
        ( RedColorList[i], GreenColorList[i], BlueColorList[i] );
      ctkColorPickerButton* colorPicker = new ctkColorPickerButton( this );
      colorPicker->setDisplayColorName( false );
      colorPicker->setColor( TubeDisplayColor );
      colorPicker->setWindowIconText( QVariant( TubeIdList[i] ).toString() );
      QObject::connect( colorPicker,
        SIGNAL( colorChanged( QColor) ), this,
        SLOT( onClickTubeColorPicker( QColor ) ) );
      d->TableWidget->setCellWidget( newRow, colIndex, colorPicker );

      //Is Root Column
      colIndex = d->columnIndex( "Is Root" );
      QTableWidgetItem* isRootItem = new QTableWidgetItem();
      if( IsRootList[i] )
        {
        isRootItem->setText( "Root" );
        if( d->logic()->GetSpatialObjectChildrenData
            ( d->SpatialObjectsNode, TubeIdList[i] ).size() != 0 )
          {
          QPushButton* showTreePushButton = new QPushButton( this );
          showTreePushButton->setText( "Root" );
          QMenu *menu = new QMenu( this );
          QAction* colorAction = menu->addAction( "Color Tree" );
          colorAction->setData( QVariant( TubeIdList[i] ) );
          QObject::connect( colorAction,
            SIGNAL( triggered() ), this,
            SLOT( onActionColorTree() ) );
          QAction* hideAction = menu->addAction( "Hide Tree" );
          hideAction->setData( QVariant( TubeIdList[i] ) );
          QObject::connect( hideAction,
            SIGNAL( triggered() ), this,
            SLOT( onActionHideTree() ) );
          QAction* displayAction = menu->addAction( "Propogate Display Properties" );
          displayAction->setData( QVariant( TubeIdList[i] ) );
          QObject::connect( displayAction,
            SIGNAL( triggered() ), this,
            SLOT( onActionDisplayPropertiesTree() ) );
          QAction* propogateOpacityAction = menu->addAction( "Propogate Opacity" );
          propogateOpacityAction->setData( QVariant( TubeIdList[i] ) );
          QObject::connect( propogateOpacityAction,
            SIGNAL( triggered() ), this,
            SLOT( onActionPropogateOpacityTree() ) );
          QAction* arteryAction = menu->addAction( "Progogate Artery Status" );
          arteryAction->setData( QVariant( TubeIdList[i] ) );
          QObject::connect( arteryAction,
            SIGNAL( triggered() ), this,
            SLOT( onActionPropogateArteryStatus() ) );
          showTreePushButton->setMenu( menu );

          d->TableWidget->setCellWidget( newRow, colIndex, showTreePushButton );
          }
        }
      else
        {
        isRootItem->setText( "" );
        }
      isRootItem->setFlags( isRootItem->flags() &  ~Qt::ItemIsEditable );
      d->TableWidget->setItem( newRow, colIndex, isRootItem );

      //Is Artery Column
      colIndex = d->columnIndex( "Is Artery" );
      QTableWidgetItem* isArteryItem = new QTableWidgetItem();
      if( IsArteryList[i] )
        {
        isArteryItem->setText( "Artery" );
        }
      else
        {
        isArteryItem->setText( "" );
        }
      isArteryItem->setFlags( isRootItem->flags() &  ~Qt::ItemIsEditable );
      d->TableWidget->setItem( newRow, colIndex, isArteryItem );

      //Parent Id Column
      colIndex = d->columnIndex( "Parent Id" );
      QTableWidgetItem* parentIdItem = new QTableWidgetItem();
      parentIdItem->setText( QVariant( ParentIdList[i] ).toString() );
      parentIdItem->setFlags( parentIdItem->flags() &  ~Qt::ItemIsEditable );
      d->TableWidget->setItem( newRow, colIndex, parentIdItem );
      }

    //Select rows if required
    for ( std::set< int >::iterator it = copySelectedTubeIds.begin();
      it != copySelectedTubeIds.end(); ++it )
      {
      this->SelectTube( *it );
      }
    }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onTableCellClicked
  ( QTableWidgetItem* item )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( item == 0 )
    {
    return;
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onSelectTubeColorChanged
  ( const QColor &color )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode )
    {
    return;
    }
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedRows();
  foreach ( QModelIndex index, indexList )
    {
    int tubeID = this->getTubeIDfromRowID( index.row() );
    this->SelectTube( tubeID,index.row() );
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickTubeColorPicker
  ( const QColor &color )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode )
    {
    return;
    }
  ctkColorPickerButton* colorPicker = qobject_cast< ctkColorPickerButton* >( sender() );
  if( !colorPicker )
    {
    return;
    }
  bool isNumeric;
  int tubeID = colorPicker->windowIconText().toInt( &isNumeric );
  if ( isNumeric && ChangeSpatialObjectColorMap( color, tubeID ) )
    {
    d->logic()->SetSpatialObjectColor( d->SpatialObjectsNode,
      tubeID, color.redF(), color.greenF(), color.blueF() );
    }
  int rowID = this->getRowIDfromTubeID( tubeID );
  if( this->isRowSelected( rowID,tubeID) )
    {
    this->SelectTube( tubeID, rowID );
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::ChangeTubeColor
  ( const QColor &color, int tubeID, int rowID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode )
    {
    return;
    }
  int colorIndex = d->columnIndex( "Color" );
  if( rowID < 0 )
    {
    if( tubeID < 0 )
      {
      return;
      }
    rowID = this->getRowIDfromTubeID( tubeID );
    }
  ctkColorPickerButton* t = qobject_cast< ctkColorPickerButton* >
    ( d->TableWidget->cellWidget( rowID, colorIndex ) );
  t->setColor( color );
}

//------------------------------------------------------------------------------
bool qSlicerInteractiveTubesToTreeTableWidget::ChangeSpatialObjectColorMap
  ( const QColor &color, int tubeID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode )
    {
    return false;
    }
  char colorMapName[80];
  strcpy( colorMapName, d->SpatialObjectsNode->GetName() );
  strcat( colorMapName, "_TubeColor" );
  vtkMRMLNode* colorNode1 =
    d->SpatialObjectsDisplayNode->GetScene()->GetFirstNodeByName( colorMapName );
  vtkMRMLProceduralColorNode* colorNode =
    vtkMRMLProceduralColorNode::SafeDownCast( colorNode1 );
  if( colorNode )
    {
    vtkColorTransferFunction* colorMap = colorNode->GetColorTransferFunction();
    colorMap->AddRGBPoint( tubeID, color.redF(), color.greenF(), color.blueF() );
    d->SpatialObjectsDisplayNode->SetAndObserveColorNodeID( colorNode->GetID() );
    return true;
    }
  return false;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickHorizontalHeader( int column )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  d->TableWidget->sortByColumn( column );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickDeleteSelected()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  const std::set< int > selectedTubeIds = d->SpatialObjectsNode->GetSelectedTubeIds();
  if( selectedTubeIds.size() != 0 )
    {
    d->logic()->deleteTubeFromSpatialObject( d->SpatialObjectsNode );
    buildTubeDisplayTable();
    }
  //delete corresponding markups.
  vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
  if( currentMarkupsNode )
    {
    int numMarups = currentMarkupsNode->GetNumberOfMarkups();
    for( int index = numMarups - 1; index >= 0; index-- )
      {
      QString indexLabel = QString::fromStdString
        ( currentMarkupsNode->GetNthMarkupLabel( index ) );
      bool isNumeric;
      int indexTubeId = indexLabel.toInt( &isNumeric );
      if( isNumeric && selectedTubeIds.find( indexTubeId ) != selectedTubeIds.end() )
        {
        currentMarkupsNode->RemoveMarkup( index );
        }
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onTableSelectionChanged()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  const std::set< int > selectedTubeIds = d->SpatialObjectsNode->GetSelectedTubeIds();
  for ( std::set< int >::iterator it = selectedTubeIds.begin();
    it != selectedTubeIds.end(); ++it )
    {
    int tubeID = *it;
    int rowID = this->getRowIDfromTubeID( tubeID );
    int colorIndex = d->columnIndex( "Color" );
    ctkColorPickerButton* t = qobject_cast< ctkColorPickerButton* >
      ( d->TableWidget->cellWidget( rowID, colorIndex ) );
    QColor tubeColor = t->color();
    this->ChangeSpatialObjectColorMap( tubeColor, tubeID );
    }
  d->SpatialObjectsNode->ClearSelectedTubes();
  int tubeIDIndex = d->columnIndex( "Tube ID" );
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedRows();
  foreach( QModelIndex index, indexList )
    {
    int curRowID = index.row();
    int tubeID = this->getTubeIDfromRowID( curRowID );
    d->SpatialObjectsNode->InsertSelectedTube( tubeID );
    this->SelectTube( tubeID, curRowID );
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::hideColumn( int colID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  d->TableWidget->hideColumn( colID );
}

//------------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidget::getColumnIndex( std::string columnName )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  return ( d->columnIndex( QString( columnName.c_str() ) ) );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::SelectTube( int tubeID, int rowID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if( tubeID < 0 )
    {
    return;
    }
  if( rowID < 0 )
    {
    rowID = this->getRowIDfromTubeID( tubeID );
    if ( rowID < 0 )
      {
      return;
      }
    }

  QColor selectionColor =  d->SelectTubeColorPicker->color();
  this->ChangeSpatialObjectColorMap( selectionColor, tubeID );
  QModelIndexList indexList = d->TableWidget->selectionModel()->selectedRows();
  foreach( QModelIndex index, indexList )
    {
    if( index.row() == rowID )
      {
      return;
      }
    }
  d->TableWidget->selectRow( rowID );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::unSelectTube( int tubeID, int rowID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if( tubeID < 0 )
    {
    return;
    }
  if( rowID < 0 )
    {
    rowID = this->getRowIDfromTubeID( tubeID );
    if ( rowID < 0 )
      {
      return;
      }
    }

  int colorIndex = d->columnIndex( "Color" );
  ctkColorPickerButton* t = qobject_cast< ctkColorPickerButton* >
    ( d->TableWidget->cellWidget( rowID, colorIndex ) );
  QColor tubeColor = t->color();
  this->ChangeSpatialObjectColorMap( tubeColor, tubeID );
  if( this->isRowSelected( rowID ) )
    {
    d->TableWidget->selectRow( rowID );
    }
}

//------------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidget::getTubeIDfromRowID( int rowID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int tubeIDIndex = d->columnIndex( "Tube ID" );
  QTableWidgetItem* item = d->TableWidget->item( rowID, tubeIDIndex );
  bool isNumeric;
  int tubeID = item->text().toInt( &isNumeric );
  if( !isNumeric )
    {
    return -1;
    }
  else
    {
    return tubeID;
    }
}

//------------------------------------------------------------------------------
int qSlicerInteractiveTubesToTreeTableWidget::getRowIDfromTubeID( int tubeID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int tubeIDIndex = d->columnIndex( "Tube ID" );
  for ( int indexRow = 0; indexRow < d->TableWidget->rowCount(); indexRow++ )
    {
    QTableWidgetItem* item = d->TableWidget->item( indexRow, tubeIDIndex );
    bool isNumeric;
    int currTubeId = item->text().toInt( &isNumeric );
    if ( isNumeric && currTubeId == tubeID )
      {
      return indexRow;
      }
    }
  return -1;
}

//------------------------------------------------------------------------------
bool qSlicerInteractiveTubesToTreeTableWidget::isRowSelected( int rowID, int tubeID )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if( tubeID < 0 )
    {
    tubeID = this->getTubeIDfromRowID( rowID );
    }
  const std::set< int > selectedTubeIds = d->SpatialObjectsNode->GetSelectedTubeIds();
  return ( selectedTubeIds.find( tubeID ) != selectedTubeIds.end() );
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onNodeAddedEvent
  ( vtkObject*, vtkObject* node )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->MarkupsNode == NULL )
    {
    vtkMRMLMarkupsNode* MarkupsNode1 = vtkMRMLMarkupsNode::SafeDownCast( node );
    if ( MarkupsNode1 )
      {
      d->MarkupsNode = MarkupsNode1;
      d->logic()->setActivePlaceNodeID( d->MarkupsNode );

      this->qvtkConnect( d->MarkupsNode, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,
        this, SLOT( onNthMarkupModifiedEvent( vtkObject*, vtkObject* ) ) );
      this->qvtkConnect( d->MarkupsNode, vtkMRMLMarkupsNode::MarkupAddedEvent,
        this, SLOT( onMarkupAddEvent() ) );
      findTubeIDs( 0 );
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onNthMarkupModifiedEvent
  ( vtkObject *caller, vtkObject *callData )
{
  if ( caller == NULL || callData == NULL )
    {
    return;
    }
  int *nPtr = NULL;
  int n = -1;
  nPtr = reinterpret_cast<int *>( callData );
  if ( nPtr )
    {
    n = *nPtr;
    }
  this->findTubeIDs( n );
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onMarkupAddEvent()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->MarkupsNode )
    {
    //to call NthMarkupAddedEvent explicitly.
    vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
    int num = currentMarkupsNode->GetNumberOfMarkups();
    currentMarkupsNode->SetNthMarkupVisibility( num - 1, false );
    }
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::findTubeIDs( int index )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->MarkupsNode)
    {
    vtkMRMLMarkupsNode* currentMarkupsNode = d->MarkupsNode;
    std::string currLabel = currentMarkupsNode->GetNthMarkupLabel( index );
    std::string currAssociatedNodeID =
      currentMarkupsNode->GetNthMarkupAssociatedNodeID( index );
    if ( currAssociatedNodeID.find( "vtkMRMLSpatialObjectsNode" ) == std::string::npos )
      {
      currentMarkupsNode->SetNthMarkupVisibility( index, false );
      return;
      }
    else
      {
      double xyz[3];
      currentMarkupsNode->GetMarkupPointLPS( index, 0, xyz );
      int TubeID = d->logic()->FindNearestTube( d->SpatialObjectsNode, xyz );
      if ( TubeID == -1 )
        {
        currentMarkupsNode->RemoveMarkup( index );
        }
      else
        {
        char newLabel[30];
        itoa( TubeID, newLabel, 10 );
        for( int i = 0; i < index; i++ )
          {
          std::string currMarkupLabel = currentMarkupsNode->GetNthMarkupLabel( i );
          bool isVisibleCurrMarkup = currentMarkupsNode->GetNthMarkupVisibility( i );
          if( i != index && currMarkupLabel.compare( newLabel ) == 0 && isVisibleCurrMarkup )
            {
            currentMarkupsNode->RemoveMarkup( i );
            currentMarkupsNode->RemoveMarkup( index-1 );
            unSelectTube( TubeID );
            return;
            }
          }
        currLabel = currentMarkupsNode->GetNthMarkupLabel( index );
        if( currLabel.compare(newLabel) != 0 )
          {
          currentMarkupsNode->SetNthMarkupLabel( index, newLabel );
          }
        if( !currentMarkupsNode->GetNthMarkupVisibility( index ) )
          {
          currentMarkupsNode->SetNthMarkupVisibility( index, true );
          }
        SelectTube( TubeID );
        }
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::restoreDefaults()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( d->SpatialObjectsNode != 0 && d->SpatialObjectsDisplayNode != 0 )
    {
    int tubeIDIndex = d->columnIndex( "Tube ID" );
    int rowCount = d->TableWidget->rowCount();
    for( int i = 0; i < rowCount; i++ )
      {
      int currTubeId = this->getTubeIDfromRowID( i );
      if( currTubeId >= 0 )
        {
        std::vector< double > color;
        if ( d->SpatialObjectsNode->GetColorFromDefaultColorMap( currTubeId, color ) )
          {
          QColor defaultColor = QColor::fromRgbF( color[0], color[1], color[2] );
          ChangeTubeColor( defaultColor, -1, i );
          }
        }
      }
    if( d->MarkupsNode )
      {
      d->MarkupsNode->RemoveAllMarkups();
      }
    d->TableWidget->clearSelection();
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onShowRootsColorChanged
  ( const QColor &color )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode || !d->ShowRootsPushButton->isChecked() )
    {
    return;
    }
  int isRootIndex = d->columnIndex( "Is Root" );
  int rowCount = d->TableWidget->rowCount();
  for( int rowIndex = 0; rowIndex < rowCount; rowIndex++ )
    {
    QTableWidgetItem* item = d->TableWidget->item( rowIndex, isRootIndex );
    if( item->text() != QString("") )
      {
      int tubeID = this->getTubeIDfromRowID( rowIndex );
      this->ChangeSpatialObjectColorMap( color, tubeID );
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onPressedShowRoots()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  this->onShowRootsColorChanged( d->ShowRootsColorPicker->color() );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onReleasedShowRoots()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int isRootIndex = d->columnIndex( "Is Root" );
  int rowCount = d->TableWidget->rowCount();
  for( int rowIndex = 0; rowIndex < rowCount; rowIndex++ )
    {
    QTableWidgetItem* item = d->TableWidget->item( rowIndex, isRootIndex );
    if( item->text() != QString( "" ) )
      {
      int tubeID = this->getTubeIDfromRowID( rowIndex );
      if( this->isRowSelected( rowIndex ) )
        {
        SelectTube( tubeID, rowIndex );
        }
      else
        {
        unSelectTube( tubeID, rowIndex );
        }
      }
    }
}
//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickSelectAllRoots()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int isRootIndex = d->columnIndex( "Is Root" );
  int rowCount = d->TableWidget->rowCount();
  for( int rowIndex = 0; rowIndex < rowCount; rowIndex++ )
    {
    QTableWidgetItem* item = d->TableWidget->item( rowIndex, isRootIndex );
    if( item->text() != QString( "" ) )
      {
      int tubeID = this->getTubeIDfromRowID( rowIndex );
      SelectTube( tubeID, rowIndex );
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onShowOrphansColorChanged
  ( const QColor &color )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  if ( !d->SpatialObjectsDisplayNode || !d->ShowOrphansPushButton->isChecked() )
    {
    return;
    }

  int tubeIDIndex = d->columnIndex( "Tube ID" );
  for( int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++ )
    {
    int currTubeId = this->getTubeIDfromRowID( rowIndex );
    if( d->logic()->GetSpatialObjectOrphanStatusData
        ( d->SpatialObjectsNode, currTubeId ) )
      {
      int tubeID = this->getTubeIDfromRowID( rowIndex );
      this->ChangeSpatialObjectColorMap( d->ShowOrphansColorPicker->color(), tubeID );
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onPressedShowOrphans()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  this->onShowOrphansColorChanged( d->ShowRootsColorPicker->color() );
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onReleasedShowOrphans()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int tubeIDIndex = d->columnIndex( "Tube ID" );
  for ( int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++ )
    {
    int currTubeId = this->getTubeIDfromRowID( rowIndex );
    if ( currTubeId >= 0 )
      {
      if( d->logic()->GetSpatialObjectOrphanStatusData
          ( d->SpatialObjectsNode, currTubeId ) )
        {
        if( this->isRowSelected( rowIndex ) )
          {
          SelectTube( currTubeId, rowIndex );
          }
        else
          {
          unSelectTube( currTubeId, rowIndex );
          }
        }
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickSelectAllOrphans()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  int tubeIDIndex = d->columnIndex( "Tube ID" );
  for( int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++ )
    {
    int currTubeId = this->getTubeIDfromRowID( rowIndex );
    if( d->logic()->GetSpatialObjectOrphanStatusData
        ( d->SpatialObjectsNode, currTubeId ) )
      {
      SelectTube( currTubeId, rowIndex );
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onActionColorTree()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  QAction* pAction = qobject_cast< QAction* >( sender() );
  int tubeID;
  if( pAction )
    {
    tubeID = pAction->data().toInt();
    }
  else
    {
    return;
    }
  std::set< int > childrenIDList =
    d->logic()->GetSpatialObjectChildrenData( d->SpatialObjectsNode, tubeID );

  if( childrenIDList.size() != 0 )
    {
    QColor rootColor;
    int tubeIDIndex = d->columnIndex( "Tube ID" );
    int rowID = this->getRowIDfromTubeID( tubeID );
    int colorIndex = d->columnIndex( "Color" );
    ctkColorPickerButton* t = qobject_cast< ctkColorPickerButton* >
      ( d->TableWidget->cellWidget( rowID, colorIndex ) );
    rootColor = t->color();

    //color all the children
    std::set< int >::iterator it;
    for( int rowIndex = 0; rowIndex < d->TableWidget->rowCount(); rowIndex++ )
      {
      int currTubeId = this->getTubeIDfromRowID( rowIndex );
      it = childrenIDList.find( currTubeId );
      if( it != childrenIDList.end() )
        {
        ChangeTubeColor( rootColor, currTubeId, rowIndex );
        }
      }
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickShowHideTubes( bool value )
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  QPushButton* pButton = qobject_cast< QPushButton* >( sender() );
  QIcon pushButtonIcon;
  if( !pButton )
    {
    return;
    }
  if( value )
    {
    d->SpatialObjectsDisplayNode->ScalarVisibilityOff();
    pushButtonIcon.addFile
      ( QString::fromUtf8( ":Hide.png" ), QSize(), QIcon::Normal, QIcon::Off );
    pButton->setIcon( pushButtonIcon );
    }
  else
    {
    d->SpatialObjectsDisplayNode->ScalarVisibilityOn();
    pushButtonIcon.addFile
      ( QString::fromUtf8( ":Show.png" ), QSize(), QIcon::Normal, QIcon::Off );
    pButton->setIcon( pushButtonIcon );
    }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickApplyColor()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  const std::set< int > selectedTubeIds = d->SpatialObjectsNode->GetSelectedTubeIds();
  for( std::set< int >::iterator it = selectedTubeIds.begin();
      it != selectedTubeIds.end(); ++it )
    {
    QColor tubeColor = d->SelectTubeColorPicker->color();
    this->ChangeTubeColor( tubeColor, *it );
    }
}

//------------------------------------------------------------------------------
vtkMRMLMarkupsNode* qSlicerInteractiveTubesToTreeTableWidget::getMRMLMarkupsNode()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  return d->MarkupsNode;
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeTableWidget::onClickRefreshTable()
{
  Q_D( qSlicerInteractiveTubesToTreeTableWidget );

  this->buildTubeDisplayTable();
}