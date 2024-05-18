cmake_minimum_required(VERSION 3.28)
project(GCSDllImporter_SRC)

set(MAIN_CPP main.cpp)
set(EXTRAS_CPP extras.cpp)
set(MONITOR_CPP resourceMonitor.cpp)
set(Resources ${CMAKE_BINARY_DIR}/../resources/resources.rc)

add_library(GCS.Extras SHARED ${EXTRAS_CPP})
add_library(GCS.ResourceMonitor SHARED ${MONITOR_CPP})
add_executable(GCS.DllImporter ${MAIN_CPP} ${Resources})

set_property(TARGET GCS.Extras PROPERTY CXX_STANDARD 17)
set_property(TARGET GCS.Extras PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET GCS.ResourceMonitor PROPERTY CXX_STANDARD 17)
set_property(TARGET GCS.ResourceMonitor PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET GCS.DllImporter PROPERTY CXX_STANDARD 17)
set_property(TARGET GCS.DllImporter PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(GCS.ResourceMonitor GCS.Extras)
target_link_libraries(GCS.DllImporter GCS.Extras GCS.ResourceMonitor)