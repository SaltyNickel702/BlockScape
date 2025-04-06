cmake_minimum_required(VERSION 3.31)

project(BlockScape)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_executable(out ${SOURCES}) #add WIN32 to create the windows application w/out console


include_directories("${CMAKE_SOURCE_DIR}/include/")


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(out "${CMAKE_SOURCE_DIR}/statLib/libglfw3WIN.a")
	target_link_libraries(out "${CMAKE_SOURCE_DIR}/statLib/libgladWIN.a")
else()
	target_link_libraries(out "${CMAKE_SOURCE_DIR}/statLib/libglfw3MAC.a")
	target_link_libraries(out "${CMAKE_SOURCE_DIR}/statLib/libgladMAC.a")
endif()