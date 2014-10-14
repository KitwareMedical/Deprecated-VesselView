
#-----------------------------------------------------------------------------
set(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/Input)

#
# Check if '--python-code' / 'slicer.testing' works as expected
#

add_test(
  NAME py_nomainwindow_SlicerPythonCodeTest1
  COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
  --ignore-slicerrc --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --no-main-window
  --python-code "exit(slicer.util.EXIT_SUCCESS)"
  )

add_test(
  NAME py_nomainwindow_SlicerPythonCodeTest2
  COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
  --ignore-slicerrc --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --no-main-window
  --python-code "exit(slicer.util.EXIT_FAILURE)"
  )
set_tests_properties(py_nomainwindow_SlicerPythonCodeTest2 PROPERTIES WILL_FAIL TRUE)

add_test(
  NAME py_nomainwindow_SlicerPythonCodeTest3
  COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
  --ignore-slicerrc --testing --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --no-main-window
  --python-code "import slicer.testing; slicer.testing.exitSuccess()"
  )

add_test(
  NAME py_nomainwindow_SlicerPythonCodeTest4
  COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
  --ignore-slicerrc --testing --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --no-main-window
  --python-code "import slicer.testing; slicer.testing.exitFailure('Should fail !')"
  )
set_tests_properties(py_nomainwindow_SlicerPythonCodeTest4 PROPERTIES WILL_FAIL TRUE)

add_test(
  NAME py_nomainwindow_SlicerPythonCodeTest5
  COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
  --ignore-slicerrc --testing --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --no-main-window
  --python-code "raise Exception('Should fail !')"
  )
set_tests_properties(py_nomainwindow_SlicerPythonCodeTest5 PROPERTIES WILL_FAIL TRUE)

#
# Exercise different Slicer command line option and check that no warnings are displayed.
#
macro(nowarning_test test_name)
  add_test(
    NAME py_nowarning_${test_name}Test
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:VesselViewApp${SlicerApp_EXE_SUFFIX}>
    ${ARGN} --ignore-slicerrc --testing --python-code "exit(slicer.util.EXIT_SUCCESS)"
    )
  set(fail_regexp)
  list(APPEND fail_regexp "Failed to obtain reference to")
  list(APPEND fail_regexp "Exception AttributeError: .* instance has no attribute")
  set_tests_properties(py_nowarning_${test_name}Test PROPERTIES FAIL_REGULAR_EXPRESSION "${fail_regexp}")
endmacro()
nowarning_test(mainwindow --no-splash)
nowarning_test(mainwindow_python_interactor --show-python-interactor)
nowarning_test(mainwindow_nocli --disable-cli-modules)
nowarning_test(mainwindow_noloadable --disable-loadable-modules)
nowarning_test(mainwindow_noscripted --disable-scripted-loadable-modules)
nowarning_test(mainwindow_nocli_noloadable --disable-cli-modules --disable-loadable-modules)
nowarning_test(mainwindow_nocli_noscripted --disable-cli-modules --disable-scripted-loadable-modules)
nowarning_test(mainwindow_noloadable_noscripted --disable-loadable-modules --disable-scripted-loadable-modules)
nowarning_test(mainwindow_nocli_noloadable_noscripted --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules)
nowarning_test(nomainwindow --no-main-window)
nowarning_test(nomainwindow_python_interactor --no-main-window --show-python-interactor)
nowarning_test(nomainwindow_nocli --no-main-window --disable-cli-modules)
nowarning_test(nomainwindow_noloadable --no-main-window --disable-loadable-modules)
nowarning_test(nomainwindow_noscripted --no-main-window --disable-scripted-loadable-modules)
nowarning_test(nomainwindow_nocli_noloadable --no-main-window --disable-cli-modules --disable-loadable-modules)
nowarning_test(nomainwindow_nocli_noscripted --no-main-window --disable-cli-modules --disable-scripted-loadable-modules)
nowarning_test(nomainwindow_noloadable_noscripted --no-main-window --disable-loadable-modules --disable-scripted-loadable-modules)
nowarning_test(nomainwindow_nocli_noloadable_noscripted --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules)
nowarning_test(nomainwindow_ignoreslicerrc --no-main-window --ignore-slicerrc)

#
# SelfTests
# see http://wiki.slicer.org/slicerWiki/index.php/Documentation/4.3/Developers/Tutorials/SelfTestModule
#

macro(VESSELVIEW_ADD_PYTHON_UNITTEST)
  set(options)
  set(oneValueArgs TESTNAME_PREFIX SCRIPT)
  set(multiValueArgs SLICER_ARGS)
  cmake_parse_arguments(MY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  get_filename_component(test_name ${MY_SCRIPT} NAME_WE)
  get_filename_component(_script_source_dir ${MY_SCRIPT} PATH)
  if("${_script_source_dir}" STREQUAL "")
    set(_script_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  add_test(
    NAME py_${MY_TESTNAME_PREFIX}${test_name}
    COMMAND ${Slicer_LAUNCHER_EXECUTABLE}
    --no-splash
    --testing
    --ignore-slicerrc ${Slicer_ADDITIONAL_LAUNCHER_SETTINGS}
    --python-code "import slicer.testing\\; slicer.testing.runUnitTest(['${CMAKE_CURRENT_BINARY_DIR}', '${_script_source_dir}'], '${test_name}')"
    ${MY_SLICER_ARGS}
    )
  set_property(TEST py_${MY_TESTNAME_PREFIX}${test_name} PROPERTY RUN_SERIAL TRUE)
endmacro()

if(Slicer_USE_PYTHONQT)

  # add as unit test for use at build/test time

  # add as hidden module for use at run time
  set(KIT_PYTHON_SCRIPTS
    )

  set(KIT_PYTHON_RESOURCES
    )

  ctkMacroCompilePythonScript(
    TARGET_NAME ApplicationSelfTests
    SCRIPTS "${KIT_PYTHON_SCRIPTS}"
    #RESOURCES "${KIT_PYTHON_RESOURCES}"
    DESTINATION_DIR ${Slicer_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
    INSTALL_DIR ${Slicer_INSTALL_QTSCRIPTEDMODULES_LIB_DIR}
    NO_INSTALL_SUBDIR
    )

endif()
