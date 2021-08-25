# CMakeList.txt : CMake project for DBCD, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_C_STANDARD  11)
set (CMAKE_C_STANDARD_REQUIRED  ON)

include(./cmake/CMakeLib.txt)

project("DBCD")

# Add the found include directories to our include list.
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include" )

#library files
collect_files(BlizzardDatabaseLib_source src true "*.cpp" "")
collect_files(BlizzardDatabaseLib_headers include true "*.h;*.hpp;*.inl" "")

#testConsole files
collect_files(TestConsole_source test false "*.cpp" "")
collect_files(TestConsole_headers test false "*.h;*.hpp;*.inl" "")

add_library(
    BlizzardDatabaseLib  
    ${BlizzardDatabaseLib_source} 
    ${BlizzardDatabaseLib_headers} 
)

add_executable(
    TestConsole 
    ${TestConsole_source} 
    ${TestConsole_headers}
    )

target_link_libraries(BlizzardDatabaseLib)
target_link_libraries(TestConsole BlizzardDatabaseLib)