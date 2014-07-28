#!/bin/sh

MachineName=melcor
BuildType=Release
CTestCommand=~/CMake/cmake-2.8.10.2-Linux-i386/bin/ctest
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
  # Get new nightly script
  cp -f ./Utilities/Dashboards/VesselView-${MachineName}.sh ..
  chmod +x ./Utilities/Dashboards/VesselView-${MachineName}.sh

  # Run new nightly script without updating again
  ./Utilities/Dashboards/VesselView-${MachineName}.sh NoUpdate

else

# Run the nightly
echo "Running nightly ctest"
${CTestCommand} -D Nightly -D SITE_CTEST_MODE:STRING=Nightly -D SITE_BUILD_TYPE:STRING=${BuildType} -S Utilities/Dashboards/VesselView-${MachineName}-master-nightly.cmake -V -VV -O VesselView-${MachineName}-master-nightly.log

fi

echo "Ending TubeTK dashboard script"
