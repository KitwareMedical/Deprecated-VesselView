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

// VesselEditor Logic includes
#include <vtkSlicerVesselEditorLogic.h>

// VesselEditor includes
#include "qSlicerVesselEditorModule.h"
#include "qSlicerVesselEditorModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVesselEditorModule, qSlicerVesselEditorModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVesselEditorModulePrivate
{
public:
  qSlicerVesselEditorModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModulePrivate::qSlicerVesselEditorModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVesselEditorModule methods

//-----------------------------------------------------------------------------
qSlicerVesselEditorModule::qSlicerVesselEditorModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVesselEditorModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVesselEditorModule::~qSlicerVesselEditorModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVesselEditorModule::helpText() const
{
  return "This module can be used to make some basic editions to a vessel"
    "tree network like deletion and merging.";
}

//-----------------------------------------------------------------------------
QString qSlicerVesselEditorModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVesselEditorModule::icon() const
{
  return QIcon(":/Icons/VesselEditor.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::categories() const
{
  return QStringList() << "TubeTK";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselEditorModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVesselEditorModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerVesselEditorModule
::createWidgetRepresentation()
{
  return new qSlicerVesselEditorModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVesselEditorModule::createLogic()
{
  return vtkSlicerVesselEditorLogic::New();
}
