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

#ifndef __qAppWelcomeScreen_h
#define __qAppWelcomeScreen_h

// Qt includes
#include <QAbstractListModel>
#include <QtDeclarative/QDeclarativeView>
#include <QObject>
#include <QList>
#include <QSortFilterProxyModel>

// App includes
#include "qSlicerAppExport.h"
#include "QSlicerIO.h"
class qAppWelcomeScreenPrivate;

class Q_SLICER_APP_EXPORT qAppWelcomeScreen
  : public QDeclarativeView
{
  Q_OBJECT
  Q_PROPERTY(QStringList filesToOpenOnLoad
    READ filesToOpenOnLoad
    WRITE setFilesToOpenOnLoad
    NOTIFY filesToOpenOnLoadChanged)

public:
  typedef QDeclarativeView Superclass;
  qAppWelcomeScreen(QWidget* parent = 0);
  virtual ~qAppWelcomeScreen();

  QStringList filesToOpenOnLoad() const;
  Q_INVOKABLE void addUniqueFileToOpenOnLoad(const QString& filename);
  Q_INVOKABLE void addFileToOpenOnLoad(const QString& filename);
  Q_INVOKABLE void removeOneFileToOpenOnLoad(const QString& filename);

public slots:
  void loadModule(const QString& moduleName, int layout = -1);
  void setFilesToOpenOnLoad(const QStringList& filenames);

signals:
  /// Fired when the welcome screen is done and the workflow can start.
  void done();

  void filesToOpenOnLoadChanged();

protected slots:
  void onRecentlyLoadedFilesChanged();
  void onModelChanged();

protected:
  QScopedPointer<qAppWelcomeScreenPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qAppWelcomeScreen);
};

class qRecentFilesType
{
public:
  qRecentFilesType();
  qRecentFilesType(const QString& name, const QString& type);

  QString filename() const;
  void setFilename(const QString &name);

  QString fileType() const;
  void setFileType(const QString &type);

  bool operator==(const qRecentFilesType& other);

private:
  QString m_filename;
  QString m_fileType;
};

class qRecentFilesModel : public  QAbstractListModel
{
  Q_OBJECT

public:
  qRecentFilesModel(QObject *parent=0);

  enum RecentFilesRoles
    {
    FilenameRole = Qt::UserRole + 1,
    FileTypeRole,
    };

  void addRecentFile(const qRecentFilesType& recentFile);
  void addUniqueRecentFile(const qRecentFilesType& recentFile);

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  virtual bool removeRows(int row, int column, const QModelIndex& parent = QModelIndex());

  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
  QVector<qRecentFilesType> m_recentFiles;
};

class qRecentFilesProxyFilter : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(QStringList fileTypes READ fileTypes WRITE setFileTypes)

public:
  qRecentFilesProxyFilter(QObject *parent=0);

  QStringList fileTypes() const;

  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

  Q_INVOKABLE QString filename(int row) const;

public slots:
  void setFileTypes(QStringList& newFileTypes);

private:
  QStringList m_fileTypes;
};

#endif
