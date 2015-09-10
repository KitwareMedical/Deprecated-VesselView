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
#include <QtDeclarative/QDeclarativeView>
#include <QObject>

// App includes
#include "qSlicerAppExport.h"
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
  void loadNewData();
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

#endif
