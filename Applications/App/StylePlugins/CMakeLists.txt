set(KIT ${PROJECT_NAME}StylePlugins)
project(${KIT})

#
# See CTK/CMake/ctkMacroBuildQtPlugin.cmake for details
#

# The header '${KIT}Export.h' will be automatically configured.
set(${KIT}_EXPORT "VESSEL_VIEW_STYLE_PLUGINS_EXPORT")

# Source files
set(${KIT}_SRCS
  qVesselViewStylePlugin.cxx
  qVesselViewStylePlugin.h
  qVesselViewStyle.cxx
  qVesselViewStyle.h
)

# Headers that should run through moc
set(${KIT}_MOC_SRCS
  qVesselViewStyle.h
  )

set(${KIT}_TARGET_LIBRARIES
  qSlicerBaseQTGUI
  )

ctkMacroBuildQtStylesPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
