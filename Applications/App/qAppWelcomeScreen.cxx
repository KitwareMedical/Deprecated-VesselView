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
#include <QDebug>
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include <QMessageBox>
#include <QPointer>
#include <QSettings>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerAppMainWindow.h>
#include <qSlicerIOManager.h>
#include <qSlicerModuleSelectorToolBar.h>
#include <qSlicerLayoutManager.h>

// SlicerApp includes
#include "qAppRecentFilesModel.h"
#include "qAppRecentFilesProxyFilter.h"
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
  qSlicerAppMainWindow* mainWindow() const;

  void openFilesBeforeLoad();

protected:
  qAppWelcomeScreen* q_ptr;
  qAppRecentFilesModel model;
  qAppRecentFilesProxyFilter filter;
  QStringList filesToOpenOnLoad;
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

  q->connect(
    this->mainWindow(), SIGNAL(recentlyLoadedFilesChanged()),
    q, SLOT(onRecentlyLoadedFilesChanged()));
  this->filter.setSourceModel(&(this->model));
  this->filter.setDynamicSortFilter(true);
  q->onRecentlyLoadedFilesChanged();

  q->connect(
    &(this->filter), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
    q, SLOT(onModelChanged()));
  q->connect(
    &(this->filter), SIGNAL(rowsInserted(QModelIndex,int,int)),
    q, SLOT(onModelChanged()));
  q->connect(
    &(this->filter), SIGNAL(rowsRemoved(QModelIndex,int,int)),
    q, SLOT(onModelChanged()));
  q->connect(
    &(this->filter), SIGNAL(modelReset()),
    q, SLOT(onModelChanged()));
  q->connect(
    &(this->filter), SIGNAL(layoutChanged()),
    q, SLOT(onModelChanged()));


  q->setResizeMode(QDeclarativeView::SizeRootObjectToView);
  q->setMinimumSize(QSize(800, 500));
  q->setSource(QUrl("qrc:/WelcomeScreen.qml"));
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreenPrivate::openFilesBeforeLoad()
{
  Q_Q(qAppWelcomeScreen);

  qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
  Q_ASSERT(ioManager);

  QStringList unsuccessfullyLoadedFilenames;
  foreach (QString filename, this->filesToOpenOnLoad)
    {
    if (!ioManager->loadFile(filename))
      {
      unsuccessfullyLoadedFilenames << filename;
      }
    }
  q->setFilesToOpenOnLoad(unsuccessfullyLoadedFilenames);
}

//-----------------------------------------------------------------------------
qSlicerAppMainWindow* qAppWelcomeScreenPrivate::mainWindow() const
{
  qSlicerAppMainWindow* window =
    qobject_cast<qSlicerAppMainWindow*>(
      qSlicerApplication::application()->mainWindow());
  Q_ASSERT(window);
  return window;
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
QStringList qAppWelcomeScreen::filesToOpenOnLoad() const
{
  Q_D(const qAppWelcomeScreen);
  return d->filesToOpenOnLoad;
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::addUniqueFileToOpenOnLoad(const QString& filename)
{
  Q_D(qAppWelcomeScreen);
  if (!d->filesToOpenOnLoad.contains(filename))
    {
    this->addFileToOpenOnLoad(filename);
    }
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::addFileToOpenOnLoad(const QString& filename)
{
  Q_D(qAppWelcomeScreen);
  d->filesToOpenOnLoad << filename;
  emit this->filesToOpenOnLoadChanged();
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::removeOneFileToOpenOnLoad(const QString& filename)
{
  Q_D(qAppWelcomeScreen);
  if (!d->filesToOpenOnLoad.contains(filename))
    {
    return;
    }

  d->filesToOpenOnLoad.removeOne(filename);
  emit this->filesToOpenOnLoadChanged();
}


//-----------------------------------------------------------------------------
void qAppWelcomeScreen::setFilesToOpenOnLoad(const QStringList& filenames)
{
  Q_D(qAppWelcomeScreen);
  if (d->filesToOpenOnLoad == filenames)
    {
    return;
    }

  d->filesToOpenOnLoad = filenames;
  emit this->filesToOpenOnLoadChanged();
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::loadModule(const QString& moduleName, int layout)
{
  Q_D(qAppWelcomeScreen);

  if (layout > 0)
    {
    qSlicerApplication::application()->layoutManager()->setLayout(layout);
    }

  qSlicerAppMainWindow* slicerMainWindow =
    qobject_cast<qSlicerAppMainWindow*>(
      qSlicerApplication::application()->mainWindow());
  Q_ASSERT(slicerMainWindow);
  d->openFilesBeforeLoad();
  slicerMainWindow->moduleSelector()->selectModule(moduleName);
  emit done();
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::onRecentlyLoadedFilesChanged()
{
  Q_D(qAppWelcomeScreen);

  d->model.removeRows(0, d->model.rowCount());

  foreach(qSlicerIO::IOProperties ioProperty, d->mainWindow()->recentlyLoadedFiles())
    {
    d->model.addUniqueRecentFile(
      qAppRecentFile(
        ioProperty["fileName"].toString(),
        ioProperty["fileType"].toString()));
    }

  this->onModelChanged();
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::onModelChanged()
{
  Q_D(qAppWelcomeScreen);
  this->rootContext()->setContextProperty("recentlyLoadedFilesModel", &(d->filter));
}
