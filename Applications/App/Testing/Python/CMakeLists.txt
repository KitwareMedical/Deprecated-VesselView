
#-----------------------------------------------------------------------------
set(INPUT ${Slicer_SOURCE_DIR}/Applications/SlicerApp/Data/Input)

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

include(SlicerMacroPythonTesting)

#
# Check if '--python-scripts' / 'testing' works as expected
#
set(slicer_app_dir ${Slicer_SOURCE_DIR}/Applications/SlicerApp)
set(slicer_app_python_testing_dir ${slicer_app_dir}/Testing/Python)

slicer_add_python_test(
  SCRIPT SlicerTestingExitSuccessTest.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  SCRIPT_ARGS --no-main-window TESTNAME_PREFIX nomainwindow_
  )

slicer_add_python_test(
  SCRIPT SlicerTestingExitFailureTest.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  SCRIPT_ARGS --no-main-window TESTNAME_PREFIX nomainwindow_
  )
set_tests_properties(py_nomainwindow_SlicerTestingExitFailureTest PROPERTIES WILL_FAIL TRUE)

slicer_add_python_test(
  SCRIPT SlicerTestingExitSuccessTest.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  )

slicer_add_python_test(
  SCRIPT SlicerTestingExitFailureTest.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  )
set_tests_properties(py_SlicerTestingExitFailureTest PROPERTIES WILL_FAIL TRUE)

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
# Application tests
#

slicer_add_python_unittest(
  SCRIPT ${slicer_app_python_testing_dir}/SlicerSceneObserverTest.py
  SLICER_ARGS --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
  TESTNAME_PREFIX nomainwindow_
  )

if(VTK_DEBUG_LEAKS AND Slicer_HAS_CONSOLE_IO_SUPPORT)
  set(testname MRMLCreateNodeByClassWithoutSetReferenceCount)
  slicer_add_python_test(
    SCRIPT ${slicer_app_python_testing_dir}/${testname}.py
    SCRIPT_ARGS --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
    TESTNAME_PREFIX nomainwindow_
    )
  set_tests_properties(py_nomainwindow_${testname}
    PROPERTIES PASS_REGULAR_EXPRESSION "instances? still around"
    )
endif()

set(testname MRMLCreateNodeByClassWithSetReferenceCountMinusOne)
slicer_add_python_test(
  SCRIPT ${slicer_app_python_testing_dir}/${testname}.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  SCRIPT_ARGS --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
  TESTNAME_PREFIX nomainwindow_
  )

set(testname MRMLSceneImportAndExport)
slicer_add_python_test(
  SCRIPT ${testname}.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  TESTNAME_PREFIX mainwindow_
  )

set(testname MRMLCreateNodeByClassWithSetReferenceCountToOne)
slicer_add_python_test(
  SCRIPT ${testname}.py
  SCRIPT_DIR ${slicer_app_python_testing_dir}
  SCRIPT_ARGS --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
  TESTNAME_PREFIX nomainwindow_
  )
set_tests_properties(py_nomainwindow_${testname} PROPERTIES WILL_FAIL TRUE)

slicer_add_python_unittest(
  SCRIPT ${Slicer_SOURCE_DIR}/Base/Python/tests/test_saferef.py
  SLICER_ARGS --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules
  TESTNAME_PREFIX nomainwindow_
  )

slicer_add_python_unittest(
  SCRIPT ${Slicer_SOURCE_DIR}/Base/Python/slicer/tests/test_slicer_util_save.py
  SLICER_ARGS --no-main-window --disable-cli-modules --disable-scripted-loadable-modules DATA{${INPUT}/MR-head.nrrd}
  TESTNAME_PREFIX nomainwindow_
  )

## Test reading MGH file format types.
slicer_add_python_unittest(
  SCRIPT ${Slicer_SOURCE_DIR}/Base/Python/slicer/tests/test_slicer_mgh.py
  SLICER_ARGS --no-main-window --disable-cli-modules --disable-scripted-loadable-modules
              DATA{${INPUT}/T1.mgz}
              DATA{${INPUT}/T1_longname.mgh.gz}
              DATA{${INPUT}/T1_uncompressed.mgh}
  TESTNAME_PREFIX nomainwindow_
  )

#
# SelfTests
# see http://wiki.slicer.org/slicerWiki/index.php/Documentation/4.3/Developers/Tutorials/SelfTestModule
#


if(Slicer_USE_PYTHONQT)
  get_filename_component(DATA_PATH "../Data" ABSOLUTE)

  # add as unit test for use at build/test time../
  # Fail
  slicer_add_python_unittest(SCRIPT FailTests.py)

  # Tortuosity
  set(TortuosityLogicTests_DATA
    ${DATA_PATH}/StraightTube_test0.tre
    ${DATA_PATH}/StraightTube_test1.tre
    ${DATA_PATH}/StraightTube_test2.tre
    ${DATA_PATH}/SinusTube_test0.tre
    ${DATA_PATH}/SinusTube_test1.tre
    ${DATA_PATH}/SinusTube_test2.tre
    ${DATA_PATH}/SinusTube_test3.tre
    )
  slicer_add_python_unittest(SCRIPT TortuosityLogicTests.py
    SLICER_ARGS ${TortuosityLogicTests_DATA})

  # Welcome
  slicer_add_python_unittest(SCRIPT WelcomeModuleTests.py)

  # add as hidden module for use at run time
  set(KIT_PYTHON_SCRIPTS
    FailTests.py
    TortuosityLogicTests.py
    WelcomeModuleTests.py
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
