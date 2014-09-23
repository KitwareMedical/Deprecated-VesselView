/*==============================================================================

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDeclarativeContext>
#include <QMessageBox>
#include <QSettings>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerLayoutManager.h>

// SlicerApp includes
#include "qAppWelcomeScreen.h"

// MRML includes
#include <vtkMRMLLayoutNode.h>

//-----------------------------------------------------------------------------
class qAppWelcomeScreenPrivate
{
  Q_DECLARE_PUBLIC(qAppWelcomeScreen);
public:
  qAppWelcomeScreenPrivate(qAppWelcomeScreen& object);
  virtual ~qAppWelcomeScreenPrivate();

  virtual void init();
protected:
  qAppWelcomeScreen* q_ptr;
};

//-----------------------------------------------------------------------------
// qAppWelcomeScreenPrivate methods

qAppWelcomeScreenPrivate
::qAppWelcomeScreenPrivate(qAppWelcomeScreen& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qAppWelcomeScreenPrivate::~qAppWelcomeScreenPrivate()
{
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreenPrivate::init()
{
  Q_Q(qAppWelcomeScreen);
  q->rootContext()->setContextProperty("welcomeScreen", q);
  q->setResizeMode(QDeclarativeView::SizeRootObjectToView);
  q->setMinimumSize(QSize(640, 480));
  q->setSource(QUrl("qrc:/WelcomeScreen.qml"));
}

//-----------------------------------------------------------------------------
// qAppWelcomeScreen methods

//-----------------------------------------------------------------------------
qAppWelcomeScreen::qAppWelcomeScreen(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new qAppWelcomeScreenPrivate(*this))
{
  Q_D(qAppWelcomeScreen);
  d->init();
}

//-----------------------------------------------------------------------------
qAppWelcomeScreen::~qAppWelcomeScreen()
{
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::loadMouseAtlas()
{
  QSettings settings;
  if (!settings.contains("mouseAtlas"))
    {
    QMessageBox::critical(this, "Mouse Atlas", "No \"mouseAtlas\" in settings file.");
    return;
    }
  const QString mouseAtlasFilePath = settings.value("mouseAtlas").toString();
  const bool res = qSlicerApplication::application()->ioManager()->loadScene(mouseAtlasFilePath);
  if (!res)
    {
    QMessageBox::critical(this, "Mouse Atlas",
                          QString("Failed to read mouse atlas in ") + mouseAtlasFilePath);
    return;
    }
  emit done();
}
