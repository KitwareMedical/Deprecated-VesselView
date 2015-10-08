/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerInteractiveTubesToTreeFooBarWidget.h"
#include "ui_qSlicerInteractiveTubesToTreeFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_InteractiveTubesToTree
class qSlicerInteractiveTubesToTreeFooBarWidgetPrivate
  : public Ui_qSlicerInteractiveTubesToTreeFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerInteractiveTubesToTreeFooBarWidget);
protected:
  qSlicerInteractiveTubesToTreeFooBarWidget* const q_ptr;

public:
  qSlicerInteractiveTubesToTreeFooBarWidgetPrivate(
    qSlicerInteractiveTubesToTreeFooBarWidget& object);
  virtual void setupUi(qSlicerInteractiveTubesToTreeFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeFooBarWidgetPrivate
::qSlicerInteractiveTubesToTreeFooBarWidgetPrivate(
  qSlicerInteractiveTubesToTreeFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeFooBarWidgetPrivate
::setupUi(qSlicerInteractiveTubesToTreeFooBarWidget* widget)
{
  this->Ui_qSlicerInteractiveTubesToTreeFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeFooBarWidget
::qSlicerInteractiveTubesToTreeFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerInteractiveTubesToTreeFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerInteractiveTubesToTreeFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeFooBarWidget
::~qSlicerInteractiveTubesToTreeFooBarWidget()
{
}
