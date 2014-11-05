
set(KIT ${KIT_LIBRARY_NAME})
include(SlicerMacroSimpleTest)

set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "DEBUG_LEAKS_ENABLE_EXIT_ERROR();" )
set(KIT_TEST_SRCS
  qSlicerAppAboutDialogTest1.cxx
  qSlicerAppMainWindowTest1.cxx
  qSlicerModuleFactoryManagerTest1.cxx
  )
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KIT_TEST_SRCS}
  EXTRA_INCLUDE vtkMRMLDebugLeaksMacro.h
  )

set(target_libraries ${KIT})

add_executable(${KIT}CxxTests ${Tests})
target_link_libraries(${KIT}CxxTests  ${target_libraries})

# Add Tests
simple_test(qSlicerAppAboutDialogTest1)
simple_test(qSlicerAppMainWindowTest1)
simple_test(qSlicerModuleFactoryManagerTest1 )

# Other application tests
if(Slicer_HAS_CONSOLE_IO_SUPPORT)
  add_test(
    NAME slicer_nomainwindow_DisableModulesCommandLineOptionsTest
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${PROJECT_NAME}${SlicerApp_EXE_SUFFIX}>
    ${ARGN} --testing --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
    )
  set_tests_properties(
    slicer_nomainwindow_DisableModulesCommandLineOptionsTest
    PROPERTIES PASS_REGULAR_EXPRESSION "Number of loaded modules: 1"
    )
endif()

