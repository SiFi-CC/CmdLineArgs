cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(CmdLineArgs CXX)
string(TOUPPER "${PROJECT_NAME}" CAPS_PROJECT_NAME)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 1)
set (VERSION_PATCH 2)
set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#--- You need to tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/Modules
    $ENV{ROOTSYS}
    ${CMAKE_SOURCE_DIR}/cmake-scripts
)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS Core)
set(ROOTSYS $ENV{ROOTSYS})

include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

include(GNUInstallDirs)

file(GLOB cmdlineargs_SRCS "Cmd*.cc")
file(GLOB cmdlineargs_HDRS "Cmd*.hh")

include(c++-standards)
include(code-coverage)
include(sanitizers)

cxx_11()
add_code_coverage()

ROOT_GENERATE_DICTIONARY(G__cmdlineargs_cc ${cmdlineargs_HDRS} LINKDEF LinkDef.h)

add_library(CmdLineArgs SHARED ${cmdlineargs_SRCS} G__cmdlineargs_cc)
target_link_libraries(CmdLineArgs
    ${ROOT_LIBRARIES}
)

include_directories(BEFORE ${CMAKE_SOURCE_DIR})

add_executable(example example.cc)
target_link_libraries(example
    CmdLineArgs)
#add_executable(UnitTester test_runner.cpp)

enable_testing()
add_subdirectory(tests)

target_include_directories(CmdLineArgs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(CmdLineArgs PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${VERSION_MAJOR}
    PUBLIC_HEADER "${cmdlineargs_HDRS}"
    INTERFACE_${CMAKE_PROJECT_NAME}_MAJOR_VERSION ${VERSION_MAJOR}
)

set_property(TARGET CmdLineArgs APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${CMAKE_PROJECT_NAME}_MAJOR_VERSION
)

install(TARGETS CmdLineArgs EXPORT ${CMAKE_PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${CMAKE_PROJECT_NAME})

##### set paths
if(WIN32 AND NOT CYGWIN)
    set(DEF_CMAKE_INSTALL_CMAKEDIR cmake)
else()
    set(DEF_CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
endif()
set(CMAKE_INSTALL_CMAKEDIR ${DEF_CMAKE_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

# packaging

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

export(EXPORT ${CMAKE_PROJECT_NAME}Targets
    FILE ${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}Targets.cmake
)
configure_file(
    cmake/${CMAKE_PROJECT_NAME}Config.cmake
    ${CMAKE_PROJECT_NAME}Config.cmake
    COPYONLY
)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    FILE ${CMAKE_PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT Devel
)
