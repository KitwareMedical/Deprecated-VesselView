#============================================================================
#
# Copyright (c) Kitware Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#============================================================================

set(proj TubeTK)

# Set dependency list
set(${proj}_DEPENDENCIES SlicerExecutionModel)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

set(_source_dir ${CMAKE_BINARY_DIR}/${proj})

ExternalProject_Add(${proj}
  ${${proj}_EP_ARGS}
  GIT_REPOSITORY "${git_protocol}://github.com/KitwareMedical/TubeTK.git"
  GIT_TAG "e5db56ead465a466fb845e826c832ce63c9364e7"
  CONFIGURE_COMMAND ""
  PREFIX ${CMAKE_BINARY_DIR}/${proj}-prefix
  SOURCE_DIR ${_source_dir}
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    ${${proj}_DEPENDENCIES}
  )

list(APPEND Slicer_EXTENSION_SOURCE_DIRS ${_source_dir})

set(_tubetk_options )
macro(_set varname type value)
  set(${varname} ${value})
  list(APPEND _tubetk_options ${varname}:${type})
endmacro()

# TubeTK source dir required to skip "find_package(TubeTK REQUIRED)" in VesselView modules
_set(TubeTK_SOURCE_DIR PATH ${_source_dir})
# TubeTK general settings
_set(TubeTK_BUILD_USING_SLICER BOOL ON)
_set(TubeTK_BUILD_WITHIN_SLICER BOOL ON)
# TubeTK SlicerExecutionModel settings
_set(SlicerExecutionModel_CLI_INSTALL_RUNTIME_DESTINATION STRING ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION})
_set(SlicerExecutionModel_CLI_INSTALL_LIBRARY_DESTINATION STRING ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION})
_set(SlicerExecutionModel_CLI_INSTALL_ARCHIVE_DESTINATION STRING ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_ARCHIVE_DESTINATION})
# TubeTk modules and features explicitly disabled:
_set(TubeTK_BUILD_IMAGE_VIEWER BOOL OFF)
_set(TubeTK_USE_SUPERBUILD BOOL OFF)
_set(TubeTK_USE_JsonCpp BOOL OFF)
_set(TubeTK_USE_LIBSVM BOOL ON)
_set(TubeTK_USE_KWSTYLE BOOL OFF)
_set(TubeTK_USE_CPPCHECK BOOL OFF)
_set(TubeTK_USE_GPU_ARRAYFIRE BOOL OFF)
_set(TubeTK_USE_BOOST BOOL OFF)
_set(TubeTK_USE_JsonCpp BOOL OFF)

set(github_protocol "${git_protocol}")

mark_as_superbuild(
  VARS
    ${_tubetk_options}
    git_protocol:STRING
    github_protocol:STRING
  PROJECTS Slicer
  )

