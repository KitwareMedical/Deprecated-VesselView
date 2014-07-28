@echo off

if exist "C:\Work\D\Vvmn\*" goto deleteDir
echo No existing build directory
goto updateScripts

:deleteDir
echo Delete directory
pushd "C:\Work\D\Vvmn"
for /d %%d in (*.*) do rmdir /s /q "%%d"
del /q *.*
popd

:updateScripts
cd C:\Work\VesselView\Dashboards\VesselView
"C:\Program Files (x86)\Git\bin\git" fetch origin
"C:\Program Files (x86)\Git\bin\git" reset --hard master

:startDashboards
echo Start dashboards
call "C:\Program Files (x86)\CMake 2.8.11.2\bin\ctest.exe" -S C:\Work\VesselView\Dashboards\VesselView\Utilities\Dashboards\VesselView-errai-master-nightly.cmake -V -C Release > C:\Work\VesselView\Dashboards\Logs\VesselView-errai-master-nightly.log 2>&1
