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

#ifndef __qAppRecentFilesModel_h
#define __qAppRecentFilesModel_h

// Qt includes
#include <QAbstractListModel>
#include <QVector>

// App includes
#include "qSlicerAppExport.h"
#include "qAppRecentFile.h"

class Q_SLICER_APP_EXPORT qAppRecentFilesModel : public QAbstractListModel
{
  Q_OBJECT

public:
  qAppRecentFilesModel(QObject *parent=0);

  enum RecentFilesRoles
    {
    FilenameRole = Qt::UserRole + 1,
    FileTypeRole,
    };

  void addRecentFile(const qAppRecentFile& recentFile);
  void addUniqueRecentFile(const qAppRecentFile& recentFile);

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  virtual bool removeRows(int row, int column, const QModelIndex& parent = QModelIndex());

  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
  QVector<qAppRecentFile> m_recentFiles;
};

#endif
