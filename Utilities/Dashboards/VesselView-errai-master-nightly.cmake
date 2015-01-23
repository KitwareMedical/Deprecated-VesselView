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

set(MY_QT_VERSION "4.8.4")
set(CTEST_BUILD_CONFIGURATION "Release")
set(SCRIPT_MODE "nightly")
set(CTEST_USE_LAUNCHERS ON)
set(CTEST_BINARY_DIRECTORY "C:/Work/D/Vvmn")
set(WITH_PACKAGES ON)
set(MY_CMAKE_VERSION "3.0.2")

set(VESSELVIEW_DIR "C:/Work/VesselView/Dashboards/VesselView")
include(${VESSELVIEW_DIR}/Utilities/Dashboards/VesselView-errai-dashboard.cmake)
