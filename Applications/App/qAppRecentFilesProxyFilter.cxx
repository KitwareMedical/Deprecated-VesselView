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

// Slicer includes
#include "qAppRecentFilesModel.h"
#include "qAppRecentFilesProxyFilter.h"

//-----------------------------------------------------------------------------
qAppRecentFilesProxyFilter::qAppRecentFilesProxyFilter(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

//-----------------------------------------------------------------------------
QStringList qAppRecentFilesProxyFilter::fileTypes() const
{
  return this->m_fileTypes;
}

//-----------------------------------------------------------------------------
void qAppRecentFilesProxyFilter::setFileTypes(QStringList& newFileTypes)
{
  this->m_fileTypes = newFileTypes;
  this->invalidateFilter();
}

//-----------------------------------------------------------------------------
QString qAppRecentFilesProxyFilter::filename(int row) const
{
  QModelIndex index = this->index(row, 0);
  return this->data(index, qAppRecentFilesModel::FilenameRole).toString();
}

//-----------------------------------------------------------------------------
bool qAppRecentFilesProxyFilter::hasAtLeastOneEntry() const
{
  return this->hasIndex(0, 0);
}

//-----------------------------------------------------------------------------
bool qAppRecentFilesProxyFilter
::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  QModelIndex index = this->sourceModel()->index(sourceRow, 0, sourceParent);
  QVariant type =
    this->sourceModel()->data(index, qAppRecentFilesModel::FileTypeRole);
  return this->m_fileTypes.contains(type.toString());
}
