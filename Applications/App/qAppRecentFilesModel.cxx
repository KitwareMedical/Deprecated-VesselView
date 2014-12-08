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
#include <QHash>

// Self includes:
#include "qAppRecentFilesModel.h"

//-----------------------------------------------------------------------------
qAppRecentFilesModel
::qAppRecentFilesModel(QObject *parent) : QAbstractListModel(parent)
{
  QHash<int, QByteArray> roles;
  roles[FilenameRole] = "filename";
  roles[FileTypeRole] = "filetype";
  this->setRoleNames(roles);
}

//-----------------------------------------------------------------------------
void qAppRecentFilesModel::addRecentFile(const qAppRecentFile& recentFile)
{
  this->beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_recentFiles << recentFile;
  this->endInsertRows();
}

//-----------------------------------------------------------------------------
void qAppRecentFilesModel
::addUniqueRecentFile(const qAppRecentFile& recentFile)
{
  if (!this->m_recentFiles.contains(recentFile))
    {
    this->addRecentFile(recentFile);
    }
}

//-----------------------------------------------------------------------------
int qAppRecentFilesModel::rowCount(const QModelIndex & parent) const
{
  return this->m_recentFiles.count();
}


//-----------------------------------------------------------------------------
bool qAppRecentFilesModel
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
QVariant qAppRecentFilesModel::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() > this->m_recentFiles.count())
    return QVariant();

  const qAppRecentFile& recentFile = m_recentFiles[index.row()];
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
