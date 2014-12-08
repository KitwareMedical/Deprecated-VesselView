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
  qRecentFilesModel model;
  qRecentFilesProxyFilter filter;
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

  foreach(qSlicerIO::IOProperties ioProperty, d->mainWindow()->recentlyLoadedFiles())
    {
    d->model.addUniqueRecentFile(
      qRecentFilesType(
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

//-----------------------------------------------------------------------------
qRecentFilesType::qRecentFilesType()
{
  m_filename = "";
  m_fileType = "";
}
//-----------------------------------------------------------------------------
qRecentFilesType::qRecentFilesType(const QString& name, const QString& type)
{
  this->setFilename(name);
  this->setFileType(type);
}

//-----------------------------------------------------------------------------
QString qRecentFilesType::filename() const
{
  return m_filename;
}

//-----------------------------------------------------------------------------
void qRecentFilesType::setFilename(const QString &name)
{
  if (name == this->m_filename)
    {
    return;
    }

  this->m_filename = name;
}

//-----------------------------------------------------------------------------
QString qRecentFilesType::fileType() const
{
  return m_fileType;
}

//-----------------------------------------------------------------------------
void qRecentFilesType::setFileType(const QString &type)
{
  if (type == this->m_fileType)
    {
    return;
    }

  this->m_fileType = type;
}

//-----------------------------------------------------------------------------
bool qRecentFilesType::operator==(const qRecentFilesType& other)
{
  return this->filename() == other.filename()
    && this->fileType() == other.fileType();
}

//-----------------------------------------------------------------------------
qRecentFilesModel::qRecentFilesModel(QObject *parent) : QAbstractListModel(parent)
{
  QHash<int, QByteArray> roles;
  roles[FilenameRole] = "filename";
  roles[FileTypeRole] = "filetype";
  this->setRoleNames(roles);
}

//-----------------------------------------------------------------------------
void qRecentFilesModel::addRecentFile(const qRecentFilesType& recentFile)
{
  this->beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_recentFiles << recentFile;
  this->endInsertRows();
}

//-----------------------------------------------------------------------------
void qRecentFilesModel::addUniqueRecentFile(const qRecentFilesType& recentFile)
{
  if (!this->m_recentFiles.contains(recentFile))
    {
    this->addRecentFile(recentFile);
    }
}

//-----------------------------------------------------------------------------
int qRecentFilesModel::rowCount(const QModelIndex & parent) const
{
  return this->m_recentFiles.count();
}


//-----------------------------------------------------------------------------
bool qRecentFilesModel
::removeRows(int row, int count, const QModelIndex& parent)
{
  if (row < 0 || row > this->m_recentFiles.count())
    {
    return false;
    }

  this->beginRemoveRows(parent, row, row + count);
  this->m_recentFiles.remove(row, count);
  this->endRemoveRows();
  return true;
}

//-----------------------------------------------------------------------------
QVariant qRecentFilesModel::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() > this->m_recentFiles.count())
    return QVariant();

  const qRecentFilesType& recentFile = m_recentFiles[index.row()];
  if (role == FilenameRole)
    {
    return recentFile.filename();
    }
  else if (role == FileTypeRole)
    {
    return recentFile.fileType();
    }
  return QVariant();
}

//-----------------------------------------------------------------------------
qRecentFilesProxyFilter::qRecentFilesProxyFilter(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

//-----------------------------------------------------------------------------
QStringList qRecentFilesProxyFilter::fileTypes() const
{
  return this->m_fileTypes;
}

//-----------------------------------------------------------------------------
void qRecentFilesProxyFilter::setFileTypes(QStringList& newFileTypes)
{
  this->m_fileTypes = newFileTypes;
  this->invalidateFilter();
}

//-----------------------------------------------------------------------------
QString qRecentFilesProxyFilter::filename(int row) const
{
  QModelIndex index = this->index(row, 0);
  return this->data(index, qRecentFilesModel::FilenameRole).toString();
}

//-----------------------------------------------------------------------------
bool qRecentFilesProxyFilter::hasAtLeastOneEntry() const
{
  return this->hasIndex(0, 0);
}

//-----------------------------------------------------------------------------
bool qRecentFilesProxyFilter
::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  QModelIndex index = this->sourceModel()->index(sourceRow, 0, sourceParent);
  QVariant type =
    this->sourceModel()->data(index, qRecentFilesModel::FileTypeRole);
  return this->m_fileTypes.contains(type.toString());
}
