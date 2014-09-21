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

#ifndef __qAppLayoutViewFactory_h
#define __qAppLayoutViewFactory_h

// CTK includes
#include "ctkLayoutViewFactory.h"

// App includes
#include "qSlicerAppExport.h"
class qAppLayoutViewFactoryPrivate;

class Q_SLICER_APP_EXPORT qAppLayoutViewFactory
 : public ctkLayoutViewFactory
{
  Q_OBJECT
public:
  typedef ctkLayoutViewFactory Superclass;
  qAppLayoutViewFactory(QObject* parent);
  virtual ~qAppLayoutViewFactory();

  /// Reimplemented to support 'welcome'
  virtual QStringList supportedElementNames()const;

protected:
  QScopedPointer<qAppLayoutViewFactoryPrivate> d_ptr;

  /// Reimplemented to instantiate the welcome screen widget.
  virtual QWidget* createViewFromXML(QDomElement layoutElement);

private:
  Q_DECLARE_PRIVATE(qAppLayoutViewFactory);
};

#endif
