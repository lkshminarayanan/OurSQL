cmake_minimum_required(VERSION 2.6)
# Avoid warnings in higher versions
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.6)
  cmake_policy(VERSION 2.8)
endif()

message(STATUS "Running cmake version ${CMAKE_VERSION}")

if(NOT "${UNIX}" EQUAL 1)
  message(FATAL_ERROR "OurSQL runs only on Unix machines.")
endif()

project(oursql)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "-g -Wall")

option(WITH_DEBUG "Use dbug" OFF)
if(WITH_DEBUG)
  add_definitions(-DDEBUG)
endif()

#directories to look for include files
include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/src/dbengine")
include_directories("${CMAKE_SOURCE_DIR}/src/dbengine/datamodels")

#directories to look for link libraries
link_directories("${CMAKE_SOURCE_DIR}/src")
link_directories("${CMAKE_SOURCE_DIR}/src/dbengine")
link_directories("${CMAKE_SOURCE_DIR}/src/dbengine/datamodels")

#compiling and linking source files
add_subdirectory(src)
