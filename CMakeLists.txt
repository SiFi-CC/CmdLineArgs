cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(CmdLineArgs
     VERSION 2.2.0
     LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

#--- You need to tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/Modules
    $ENV{ROOTSYS}
    ${CMAKE_SOURCE_DIR}/cmake-scripts
)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS Core)

include(GNUInstallDirs)

file(GLOB cmdlineargs_SRCS CmdLineArg.cc CmdLineConfig.cc CmdLineOption.cc)
file(GLOB cmdlineargs_HDRS CmdLineArg.hh CmdLineConfig.hh CmdLineOption.hh)

include(c++-standards)
include(code-coverage)
include(sanitizers)

cxx_17()
add_code_coverage()

ROOT_GENERATE_DICTIONARY(G__cmdlineargs_cc ${cmdlineargs_HDRS} LINKDEF LinkDef.h)

add_library(CmdLineArgs SHARED ${cmdlineargs_SRCS} G__cmdlineargs_cc)
target_link_libraries(CmdLineArgs
    ROOT::Hist
)

add_library(SiFi::CmdLineArgs ALIAS CmdLineArgs)

#include_directories(BEFORE ${CMAKE_SOURCE_DIR})

target_include_directories(CmdLineArgs
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(CmdLineArgs
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${cmdlineargs_HDRS}"
        INTERFACE_${CMAKE_PROJECT_NAME}_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
)

set_property(TARGET CmdLineArgs APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${CMAKE_PROJECT_NAME}_MAJOR_VERSION
)

install(TARGETS CmdLineArgs
    EXPORT ${CMAKE_PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


add_executable(example example.cc)
target_link_libraries(example
    CmdLineArgs)

option(ENABLE_TESTING "Build tests" ON)

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

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
export(PACKAGE ${CMAKE_PROJECT_NAME})

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    FILE ${CMAKE_PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    FILE ${CMAKE_PROJECT_NAME}Targets.cmake
    NAMESPACE SiFi::
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR} COMPONENT dev
)


include(CMakePackageConfigHelpers)
configure_package_config_file(Config.cmake.in ${CMAKE_PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR)

write_basic_package_version_file(
    ${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(
    FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)
