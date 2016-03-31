project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${vtkSlicerSpatialObjectsModuleMRML_INCLUDE_DIRS}
  ${vtkSlicerSpatialObjectsModuleLogic_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  qSlicer${MODULE_NAME}TableWidget.cxx
  qSlicer${MODULE_NAME}TableWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicer${MODULE_NAME}TableWidget.h
  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicer${MODULE_NAME}TableWidget.ui
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  vtkSlicerSpatialObjectsModuleLogic
  ${MRML_LIBRARIES}
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )
