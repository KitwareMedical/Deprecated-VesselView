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

#-----------------------------------------------------------------------------
set(MODULE_NAME VesselView)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
set(MODULE_PYTHON_SCRIPTS
  Workflow.py
  Widgets/__init__.py
  Widgets/WorkflowStep.py
  Widgets/InitialStep.py
  Widgets/LoadDataStep.py
  Widgets/RegisterStep.py
  Widgets/RegisterWidget.py
  Widgets/ResampleStep.py
  Widgets/ResampleWidget.py
  Widgets/SegmentationStep.py
  Widgets/SegmentationWidget.py
  Widgets/VesselEnhancementStep.py
  Widgets/ExtractSeedsStep.py
  Widgets/VesselExtractionStep.py
  )

set(MODULE_PYTHON_RESOURCES
  Widgets/Resources/Icons/Brain_Vessels.png
  Widgets/Resources/Icons/DesktopIcon.png
  Widgets/Resources/Icons/Heart_Vessels.png
  Widgets/Resources/Icons/Liver_Vessels.png
  Widgets/Resources/UI/ExtractSeedsStep.ui
  Widgets/Resources/UI/InitialStep.ui
  Widgets/Resources/UI/LoadDataStep.ui
  Widgets/Resources/UI/RegisterStep.ui
  Widgets/Resources/UI/RegisterWidget.ui
  Widgets/Resources/UI/ResampleStep.ui
  Widgets/Resources/UI/ResampleWidget.ui
  Widgets/Resources/UI/SegmentationStep.ui
  Widgets/Resources/UI/SegmentationWidget.ui
  Widgets/Resources/UI/VesselEnhancementStep.ui
  Widgets/Resources/UI/VesselExtractionStep.ui
  Widgets/Resources/UI/WorkflowSettingsPanel.ui
  )

#-----------------------------------------------------------------------------
# Resources
set (${MODULE_NAME}_PRESET_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)

file(COPY ${${MODULE_NAME}_PRESET_RESOURCES_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_SHARE_DIR}/Workflow
  )

install(
  DIRECTORY ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_SHARE_DIR}/Workflow
  DESTINATION ${Slicer_INSTALL_QTSCRIPTEDMODULES_SHARE_DIR} COMPONENT Runtime)

#-----------------------------------------------------------------------------
ctkMacroCompilePythonScript(
  TARGET_NAME ${MODULE_NAME}
  SCRIPTS "${MODULE_PYTHON_SCRIPTS}"
  RESOURCES "${MODULE_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
  INSTALL_DIR ${Slicer_INSTALL_QTSCRIPTEDMODULES_LIB_DIR}
  NO_INSTALL_SUBDIR
  )


#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  # this will regiester the unittest subclass in the main script
  # as a ctest (note that it is also usable as a runtime test).
  slicer_add_python_unittest(SCRIPT Workflow.py)
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
