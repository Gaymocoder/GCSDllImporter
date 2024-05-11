cmake_minimum_required(VERSION 3.28)
project(GCSDllImporter_SRC)

set(MAIN_CPP main.cpp)
set(MONITOR_CPP resourceMonitor.cpp)

add_library(GCS.ResourceMonitor SHARED ${MONITOR_CPP})
add_executable(GCSDllImporter ${MAIN_CPP})

target_link_libraries(GCSDllImporter GCS.ResourceMonitor)