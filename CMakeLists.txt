cmake_minimum_required(VERSION 3.28)
project(GCSDllImporter)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

include_directories(include)
add_subdirectory(src/GCSDllImporter)