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

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp")

# Add the found include directories to our include list.
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src" )
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/DatabaseDefinitionsLib")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/thirdparty/SQLiteCpp/include")

#DbDefslibrary
collect_files(DbDefsLib_source src/DatabaseDefinitionsLib false "*.cpp" "")
collect_files(DbDefsLib_headers src/DatabaseDefinitionsLib false "*.h;*.hpp;*.inl" "")

#library files
collect_files(DBFileReaderLib_source src false "*.cpp" "")
collect_files(DBFileReaderLib_headers src false "*.h;*.hpp;*.inl" "")

#testConsole files
collect_files(TestConsole_source test false "*.cpp" "")
collect_files(TestConsole_headers test false "*.h;*.hpp;*.inl" "")

add_library(DbDefsLib  ${DbDefsLib_source} ${DbDefsLib_headers})
add_library(DBFileReaderLib  ${DBFileReaderLib_source} ${DBFileReaderLib_headers})

add_executable(TestConsole ${TestConsole_source} ${TestConsole_headers})

target_link_libraries(DbDefsLib PRIVATE SQLiteCpp)
target_link_libraries(DBFileReaderLib DbDefsLib)
target_link_libraries(TestConsole DBFileReaderLib)