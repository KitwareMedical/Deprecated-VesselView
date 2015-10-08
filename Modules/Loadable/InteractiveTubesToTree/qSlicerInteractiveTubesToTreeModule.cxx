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

// Qt includes
#include <QtPlugin>

// InteractiveTubesToTree Logic includes
#include <vtkSlicerInteractiveTubesToTreeLogic.h>

// InteractiveTubesToTree includes
#include "qSlicerInteractiveTubesToTreeModule.h"
#include "qSlicerInteractiveTubesToTreeModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerInteractiveTubesToTreeModule, qSlicerInteractiveTubesToTreeModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerInteractiveTubesToTreeModulePrivate
{
public:
  qSlicerInteractiveTubesToTreeModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModulePrivate::qSlicerInteractiveTubesToTreeModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModule methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModule::qSlicerInteractiveTubesToTreeModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerInteractiveTubesToTreeModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModule::~qSlicerInteractiveTubesToTreeModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerInteractiveTubesToTreeModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerInteractiveTubesToTreeModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerInteractiveTubesToTreeModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerInteractiveTubesToTreeModule::icon() const
{
  return QIcon(":/Icons/InteractiveTubesToTree.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerInteractiveTubesToTreeModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerInteractiveTubesToTreeModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerInteractiveTubesToTreeModule
::createWidgetRepresentation()
{
  return new qSlicerInteractiveTubesToTreeModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerInteractiveTubesToTreeModule::createLogic()
{
  return vtkSlicerInteractiveTubesToTreeLogic::New();
}
