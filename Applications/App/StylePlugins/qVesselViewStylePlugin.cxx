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

  This file was originally developed by Johan Andruejol, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QStringList>
#include <QStyle>

// qMRML includes
#include "qVesselViewStyle.h"
#include "qVesselViewStylePlugin.h"

// --------------------------------------------------------------------------
// qVesselViewStylePlugin methods

//-----------------------------------------------------------------------------
qVesselViewStylePlugin::qVesselViewStylePlugin()
{
}

//-----------------------------------------------------------------------------
qVesselViewStylePlugin::~qVesselViewStylePlugin()
{
}

//-----------------------------------------------------------------------------
QStyle* qVesselViewStylePlugin::create( const QString & key )
{

  if (key.compare("VesselView", Qt::CaseInsensitive) == 0)
    {
    return new qVesselViewStyle();
    }
  return 0;
}

//-----------------------------------------------------------------------------
QStringList qVesselViewStylePlugin::keys() const
{
  return QStringList() << "VesselView";
}

Q_EXPORT_PLUGIN2(qVesselViewStylePlugin, qVesselViewStylePlugin)
