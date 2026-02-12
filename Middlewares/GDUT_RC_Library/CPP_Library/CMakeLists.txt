#######################################################################
# The Embedded Template Library (https://www.etlcpp.com/)
#######################################################################
cmake_minimum_required(VERSION 3.10)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers.cmake)

set(MSG_PREFIX "gtl |")
etl_determine_version_with_git(${GIT_DIR_LOOKUP_POLICY})
if(NOT GDUT_VERSION)
    etl_determine_version_with_file("version.txt")
endif()

project(gtl VERSION ${GDUT_VERSION} LANGUAGES CXX)

# There is a bug on old gcc versions for some targets that causes all system headers
# to be implicitly wrapped with 'extern "C"'
# Users can add set(NO_SYSTEM_INCLUDE ON) to their top level CMakeLists.txt to work around this.
option(NO_SYSTEM_INCLUDE "Do not include with -isystem" OFF)
if (NO_SYSTEM_INCLUDE)
    set(INCLUDE_SPECIFIER "")
else()
    set(INCLUDE_SPECIFIER "SYSTEM")
endif()

add_library(${PROJECT_NAME} INTERFACE)
# This allows users which use the add_subdirectory or FetchContent
# to use the same target as users which use find_package
add_library(gtl::gtl ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME} ${INCLUDE_SPECIFIER} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

target_link_libraries(${PROJECT_NAME} INTERFACE)
