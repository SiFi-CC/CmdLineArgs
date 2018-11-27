cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(CmdLineArgs CXX)
string(TOUPPER "${PROJECT_NAME}" CAPS_PROJECT_NAME)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (VERSION_PATCH 1)
set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#--- You need to tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/Modules $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS Core)
set(ROOTSYS $ENV{ROOTSYS})

include(GNUInstallDirs)

include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

file(GLOB so_SRCS "*.cc")
file(GLOB so_HDRS "*.hh")

ROOT_GENERATE_DICTIONARY(G__cmdlineargs_cc ${so_HDRS} LINKDEF LinkDef.h)

add_library(CmdLineArgs SHARED ${so_SRCS} G__cmdlineargs_cc)
target_link_libraries(CmdLineArgs
    ${ROOT_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR})

add_executable(example example.cc)
target_link_libraries(example
    CmdLineArgs)

set_target_properties(CmdLineArgs PROPERTIES
    SOVERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${so_HDRS}"
)

install(TARGETS CmdLineArgs
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
