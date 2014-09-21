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
#include <QSlider>

// SlicerApp includes
#include "qAppLayoutViewFactory.h"
#include "qAppWelcomeScreen.h"

//-----------------------------------------------------------------------------
class qAppLayoutViewFactoryPrivate
{
  Q_DECLARE_PUBLIC(qAppLayoutViewFactory);
public:
  qAppLayoutViewFactoryPrivate(qAppLayoutViewFactory& object);
  virtual ~qAppLayoutViewFactoryPrivate();

  virtual void init();
protected:
  qAppLayoutViewFactory* q_ptr;
};

//-----------------------------------------------------------------------------
// qAppLayoutViewFactoryPrivate methods

qAppLayoutViewFactoryPrivate
::qAppLayoutViewFactoryPrivate(qAppLayoutViewFactory& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qAppLayoutViewFactoryPrivate::~qAppLayoutViewFactoryPrivate()
{
}

//-----------------------------------------------------------------------------
void qAppLayoutViewFactoryPrivate::init()
{
}

//-----------------------------------------------------------------------------
// qAppLayoutViewFactory methods

//-----------------------------------------------------------------------------
qAppLayoutViewFactory::qAppLayoutViewFactory(QObject* parent)
  : Superclass(parent)
  , d_ptr(new qAppLayoutViewFactoryPrivate(*this))
{
  Q_D(qAppLayoutViewFactory);
  d->init();
}

//-----------------------------------------------------------------------------
qAppLayoutViewFactory::~qAppLayoutViewFactory()
{
}

//-----------------------------------------------------------------------------
QStringList qAppLayoutViewFactory::supportedElementNames()const
{
  QStringList res;
  res << "welcome";
  return res;
}

//---------------------------------------------------------------------------
QWidget* qAppLayoutViewFactory::createViewFromXML(QDomElement layoutElement)
{
  Q_ASSERT(layoutElement.tagName() == "welcome");
  qAppWelcomeScreen* welcomeScreen = new qAppWelcomeScreen;
  return welcomeScreen;
}
