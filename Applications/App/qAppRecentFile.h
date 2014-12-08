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

#ifndef __qAppRecentFile_h
#define __qAppRecentFile_h

// Qt includes
#include <QString>

// App includes
#include "qSlicerAppExport.h"

class Q_SLICER_APP_EXPORT qAppRecentFile
{
public:
  qAppRecentFile();
  qAppRecentFile(const QString& name, const QString& type);

  QString filename() const;
  void setFilename(const QString &name);

  QString fileType() const;
  void setFileType(const QString &type);

  bool operator==(const qAppRecentFile& other);

private:
  QString m_filename;
  QString m_fileType;
};

#endif
