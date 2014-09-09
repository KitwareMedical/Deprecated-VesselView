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

#
# TubeTK
#
set(proj TubeTK)

# Set dependency list
# TubeTK depends on all of these so we are sure it's built after them
set(${proj}_DEPENDENCIES ITKv4 VTKv6 CTK SlicerExecutionModel)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED ${proj}_DIR AND NOT EXISTS ${${proj}_DIR})
  message(FATAL_ERROR "${proj}_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED ${proj}_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

  set(${proj}_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${${proj}_DIR}
    GIT_REPOSITORY "https://github.com/KitwareMedical/TubeTK.git"
    GIT_TAG "e38fb74b49fbf9559aecb57e11506c97514e7346"
    CMAKE_CACHE_ARGS
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DBUILD_TESTING:BOOL=${BUILD_TESTING}
      # Need to build JsonCpp
      -DTubeTK_USE_SUPERBUILD:BOOL=ON
      # Slicer_SOURCE_DIR is used when generating UseTubeTK.cmake
      -DSlicer_SOURCE_DIR:PATH=${Slicer_SOURCE_DIR}
      # Slicer is not available yet (no SlicerConfig.cmake generated)
      -DTubeTK_USE_Slicer:BOOL=OFF
      -DUSE_SYSTEM_SlicerExecutionModel:BOOL=ON
      -DSlicerExecutionModel_DIR:PATH=${CMAKE_BINARY_DIR}/SlicerExecutionModel-build
      -DUSE_SYSTEM_ITK:BOOL=ON
      -DITK_DIR:PATH=${ITK_DIR}
      -DTubeTK_USE_BOOST:BOOL=OFF
      -DTubeTK_USE_VTK:BOOL=ON
      -DUSE_SYSTEM_VTK:BOOL=ON
      -DVTK_DIR:PATH=${VTK_DIR}
      -DTubeTK_USE_QT:BOOL=ON
      -DTubeTK_REQUIRED_QT_VERSION:STRING=${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      -DTubeTK_USE_CTK:BOOL=ON
      -DTubeTK_USE_LIBSVM:BOOL=OFF
      -DUSE_SYSTEM_CTK:BOOL=ON
      -DCTK_DIR:PATH=${CTK_DIR}
      # Build only TubeTK lib, not the modules. They will be built by Slicer.
      -DTubeTK_BUILD_ALL_MODULES:BOOL=OFF
      -DTubeTK_BUILD_APPLICATIONS:BOOL=OFF
      -DTubeTK_BUILD_SLICER_EXTENSION:BOOL=OFF
      -DTubeTK_USE_LIBSVM:BOOL=OFF
      -DTubeTK_USE_PYTHON:BOOL=OFF
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

