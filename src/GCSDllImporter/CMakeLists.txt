cmake_minimum_required(VERSION 3.28)
project(GCSDllImporter_SRC)

set(MAIN_CPP main.cpp)
set(EXTRAS_CPP extras.cpp)
set(MONITOR_CPP resourceMonitor.cpp)

add_library(GCS.Extras SHARED ${EXTRAS_CPP})
add_library(GCS.ResourceMonitor SHARED ${MONITOR_CPP})
add_executable(GCS.DllImporter ${MAIN_CPP})

target_link_libraries(GCS.ResourceMonitor GCS.Extras)
target_link_libraries(GCS.DllImporter GCS.Extras GCS.ResourceMonitor)