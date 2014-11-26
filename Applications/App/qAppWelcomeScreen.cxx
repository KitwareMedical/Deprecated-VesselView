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

protected:
  qAppWelcomeScreen* q_ptr;
  qRecentFilesModel model;
  qRecentFilesProxyFilter filter;
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
void qAppWelcomeScreen::loadModule(const QString& moduleName, int layout)
{
  if (layout > 0)
    {
    qSlicerApplication::application()->layoutManager()->setLayout(layout);
    }

  qSlicerAppMainWindow* slicerMainWindow =
    qobject_cast<qSlicerAppMainWindow*>(
      qSlicerApplication::application()->mainWindow());
  Q_ASSERT(slicerMainWindow);
  slicerMainWindow->moduleSelector()->selectModule(moduleName);
  emit done();
}

//-----------------------------------------------------------------------------
void qAppWelcomeScreen::onRecentlyLoadedFilesChanged()
{
  Q_D(qAppWelcomeScreen);
  d->filter.setDynamicSortFilter(false);
  d->model.removeRows(0, d->model.rowCount());

  foreach(qSlicerIO::IOProperties ioProperty, d->mainWindow()->recentlyLoadedFiles())
    {
    d->model.addUniqueRecentFile(
      qRecentFilesType(
        ioProperty["fileName"].toString(),
        ioProperty["fileType"].toString()));
    }
  d->filter.setSourceModel(&(d->model));
  d->filter.setDynamicSortFilter(true);

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
bool qRecentFilesProxyFilter
::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  QModelIndex index = this->sourceModel()->index(sourceRow, 0, sourceParent);
  QVariant type =
    this->sourceModel()->data(index, qRecentFilesModel::FileTypeRole);
  return this->m_fileTypes.contains(type.toString());
}
