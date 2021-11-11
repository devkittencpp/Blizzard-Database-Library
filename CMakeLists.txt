# CMakeList.txt : CMake project for DBCD, include source and define
# project specific logic here.
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_C_STANDARD  11)
SET(CMAKE_C_STANDARD_REQUIRED  ON)

INCLUDE(./cmake/cmake-function.cmake)

PROJECT("BlizzardDatabaseLib")

# Add the found include directories to our include list.
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include" )

#library files
collect_files(BlizzardDatabaseLib_source "${CMAKE_CURRENT_SOURCE_DIR}/src" true "*.cpp" "")
collect_files(BlizzardDatabaseLib_headers "${CMAKE_CURRENT_SOURCE_DIR}/include" true "*.h;*.hpp;*.inl" "")

#testConsole files
collect_files(TestConsole_source "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.cpp" "")
collect_files(TestConsole_headers "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.h;*.hpp;*.inl" "")

ADD_LIBRARY(BlizzardDatabaseLib  
    ${BlizzardDatabaseLib_source} 
    ${BlizzardDatabaseLib_headers} 
)

ADD_EXECUTABLE(TestConsole 
    ${TestConsole_source} 
    ${TestConsole_headers}
)

TARGET_LINK_LIBRARIES(BlizzardDatabaseLib)
TARGET_LINK_LIBRARIES(TestConsole BlizzardDatabaseLib)