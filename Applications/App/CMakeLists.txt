#============================================================================
#
# Copyright (c) Kitware, Inc.
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

set(APPLICATION_NAME ${Slicer_MAIN_PROJECT_APPLICATION_NAME})

project(${APPLICATION_NAME}App)

include(SlicerMacroBuildApplication)

# --------------------------------------------------------------------------
# Slicer supports more than one application (i.e. an application
# "OtherApp" in addition to "APPLICATION_NAMEApp").
#
# In that specific case, it is required to differentiate the two applications
# and specify which one should be considered as the *Main* one.
#
# This is usually done within the top level CMakeLists.txt file by setting the variable
# Slicer_MAIN_PROJECT.
#
# Within the Applications/OtherApp/CMakeLists.txt of the secondary application,
# the project name should be updated, the variable 'SlicerApp_APPLICATION_NAME'
# could also be optionnally set to a custom value.
#

if(${PROJECT_NAME} STREQUAL ${Slicer_MAIN_PROJECT})
  set(SlicerApp_APPLICATION_NAME ${APPLICATION_NAME})
endif()

# --------------------------------------------------------------------------
# Project dependencies
# --------------------------------------------------------------------------
find_package(${APPLICATION_NAME} REQUIRED)
include(${${APPLICATION_NAME}_USE_FILE})

set(${PROJECT_NAME}_VERSION_MAJOR ${${APPLICATION_NAME}_VERSION_MAJOR})
set(${PROJECT_NAME}_VERSION_MINOR ${${APPLICATION_NAME}_VERSION_MINOR})
set(${PROJECT_NAME}_VERSION_PATCH ${${APPLICATION_NAME}_VERSION_PATCH})
if(DEFINED ${APPLICATION_NAME}_VERSION_TWEAK)
  set(${PROJECT_NAME}_VERSION_TWEAK ${${APPLICATION_NAME}_VERSION_TWEAK})
endif()
if(DEFINED ${APPLICATION_NAME}_VERSION_RC)
  set(${PROJECT_NAME}_VERSION_RC ${${APPLICATION_NAME}_VERSION_RC})
endif()
set(${PROJECT_NAME}_VERSION_FULL ${${APPLICATION_NAME}_VERSION})

# --------------------------------------------------------------------------
# Gutenberg
# --------------------------------------------------------------------------
find_package(Gutenberg REQUIRED)

include(GutenbergFontAwesomeMacro)
set(FONT_AWESOME_DIR "${CMAKE_CURRENT_BINARY_DIR}")
GutenbergFontAwesomeMacro(
  ZIP_FILE "${${APPLICATION_NAME}_SOURCE_DIR}/Utilities/Logos/FontAwesome.zip"
  DESTINATION_DIR ${FONT_AWESOME_DIR}
  )

# --------------------------------------------------------------------------
# Application library
# --------------------------------------------------------------------------
set(APPLIB_NAME "q${PROJECT_NAME}")

set(APPLIB_SRCS
  qAppLayoutViewFactory.cxx
  qAppLayoutViewFactory.h
  qAppRecentFile.cxx
  qAppRecentFile.h
  qAppRecentFilesModel.cxx
  qAppRecentFilesModel.h
  qAppRecentFilesModel.h
  qAppRecentFilesProxyFilter.cxx
  qAppRecentFilesProxyFilter.h
  qAppWelcomeScreen.cxx
  qAppWelcomeScreen.h
  qSlicerAppAboutDialog.cxx
  qSlicerAppAboutDialog.h
  qSlicerAppMainWindow.cxx
  qSlicerAppMainWindow.h
  qSlicerAppMainWindow_p.h
  )

set(APPLIB_MOC_SRCS
  qAppLayoutViewFactory.h
  qAppRecentFilesModel.h
  qAppRecentFilesProxyFilter.h
  qAppWelcomeScreen.h
  qSlicerAppAboutDialog.h
  qSlicerAppMainWindow.h
  )

set(APPLIB_UI_SRCS
  Resources/UI/qSlicerAppAboutDialog.ui
  Resources/UI/qSlicerAppMainWindow.ui
  )

set(APPLIB_RESOURCES
  Resources/App.qrc
  ${qSlicerBaseQTGUI_SOURCE_DIR}/Resources/qSlicerBaseQTGUI.qrc
  ${${APPLICATION_NAME}_SOURCE_DIR}/Utilities/Qt/Resources.qrc
  ${GUTENBERG_RESOURCE_FILE}
  )

set(APPLIB_INCLUDE_DIRECTORIES
  ${Slicer_SOURCE_DIR}/Applications/SlicerApp
  ${Slicer_BINARY_DIR}/Applications/SlicerApp
  ${FONT_AWESOME_DIR}
  )

configure_file(
  qSlicerAppExport.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/qSlicerAppExport.h
  )

slicerMacroBuildAppLibrary(
  NAME ${APPLIB_NAME}
  APPLICATION_NAME ${SlicerApp_APPLICATION_NAME}
  EXPORT_DIRECTIVE "Q_SLICER_APP_EXPORT"
  FOLDER ${${PROJECT_NAME}_FOLDER}
  SRCS ${APPLIB_SRCS}
  MOC_SRCS ${APPLIB_MOC_SRCS}
  UI_SRCS ${APPLIB_UI_SRCS}
  INCLUDE_DIRECTORIES ${APPLIB_INCLUDE_DIRECTORIES}
  RESOURCES ${APPLIB_RESOURCES}
  )

# --------------------------------------------------------------------------
# Application executable
# --------------------------------------------------------------------------
# Configure launcher only for the main application
set(extra_args)
if(${PROJECT_NAME} STREQUAL ${Slicer_MAIN_PROJECT})
  set(extra_args CONFIGURE_LAUNCHER)
endif()

set(APP_SRCS
  Main.cxx
  )

slicerMacroBuildApplication(
  NAME ${PROJECT_NAME}
  APPLICATION_NAME ${${PROJECT_NAME}_APPLICATION_NAME}
  FOLDER ${${PROJECT_NAME}_FOLDER}
  APPLE_ICON_FILE "Resources/VesselView.icns"
  LAUNCHER_SPLASHSCREEN_FILE "Resources/Images/SplashScreen.png"
  WIN_ICON_FILE "Resources/VesselView.ico"
  SRCS ${APP_SRCS}
  TARGET_LIBRARIES ${APPLIB_NAME}
  TARGET_NAME_VAR "APP_TARGET_NAME"
  ${extra_args}
  )

# --------------------------------------------------------------------------

# Qt Plugins
add_subdirectory(StylePlugins)
