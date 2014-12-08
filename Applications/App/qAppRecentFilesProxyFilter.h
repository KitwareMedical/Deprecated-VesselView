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

#ifndef __qAppRecentFilesProxyFilter_h
#define __qAppRecentFilesProxyFilter_h

// Qt includes
#include <QSortFilterProxyModel>
#include <QStringList>

// App includes
#include "qSlicerAppExport.h"

class Q_SLICER_APP_EXPORT qAppRecentFilesProxyFilter : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(QStringList fileTypes READ fileTypes WRITE setFileTypes)

public:
  qAppRecentFilesProxyFilter(QObject *parent=0);

  QStringList fileTypes() const;

  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

  Q_INVOKABLE QString filename(int row) const;

  Q_INVOKABLE virtual bool hasAtLeastOneEntry() const;

public slots:
  void setFileTypes(QStringList& newFileTypes);

private:
  QStringList m_fileTypes;
};

#endif
