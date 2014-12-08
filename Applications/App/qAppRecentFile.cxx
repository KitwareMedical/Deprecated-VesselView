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

// Self includes
#include "qAppRecentFile.h"

//-----------------------------------------------------------------------------
qAppRecentFile::qAppRecentFile()
{
  m_filename = "";
  m_fileType = "";
}
//-----------------------------------------------------------------------------
qAppRecentFile
::qAppRecentFile(const QString& name, const QString& type)
{
  this->setFilename(name);
  this->setFileType(type);
}

//-----------------------------------------------------------------------------
QString qAppRecentFile::filename() const
{
  return m_filename;
}

//-----------------------------------------------------------------------------
void qAppRecentFile::setFilename(const QString &name)
{
  if (name == this->m_filename)
    {
    return;
    }

  this->m_filename = name;
}

//-----------------------------------------------------------------------------
QString qAppRecentFile::fileType() const
{
  return m_fileType;
}

//-----------------------------------------------------------------------------
void qAppRecentFile::setFileType(const QString &type)
{
  if (type == this->m_fileType)
    {
    return;
    }

  this->m_fileType = type;
}

//-----------------------------------------------------------------------------
bool qAppRecentFile::operator==(const qAppRecentFile& other)
{
  return this->filename() == other.filename()
    && this->fileType() == other.fileType();
}
