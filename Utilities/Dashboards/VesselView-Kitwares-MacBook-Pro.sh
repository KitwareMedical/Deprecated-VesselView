#!/bin/sh

MachineName=Kitwares-MacBook-Pro
BuildType=Release
CTestCommand=~/Support/cmake-3.0.1-Darwin64-universal/CMake.app/Contents/bin/ctest
DashboardDir=~/Dashboards/VesselView

echo "Running VesselView Dashboard script"
if [ $# -eq 0 ] || [ "$1" != "NoUpdate" ]; then

  echo "Updating"
  rm -rf ${DashboardDir}/VesselView-${BuildType}

  # Update Dashboard repository
  cd ${DashboardDir}/VesselView
  git fetch origin
  git reset --hard origin/master

  echo "Bootstrapping"
  # Run new nightly script without updating again
  ./Utilities/Dashboards/VesselView-${MachineName}.sh NoUpdate

else

# Run the nightly
echo "Running nightly ctest"
${CTestCommand} -D Nightly -D SITE_CTEST_MODE:STRING=Nightly -D SITE_BUILD_TYPE:STRING=${BuildType} -S Utilities/Dashboards/VesselView-${MachineName}-master-nightly.cmake -V -VV -O VesselView-${MachineName}-master-nightly.log

fi

echo "Ending VesselView dashboard script"
