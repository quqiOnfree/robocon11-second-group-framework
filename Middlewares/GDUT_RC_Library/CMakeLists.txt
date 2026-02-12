cmake_minimum_required(VERSION 3.22)

enable_language(C CXX ASM)

project(GDUT_RC_Library)

add_library(GDUT_RC_Library INTERFACE)
target_include_directories(GDUT_RC_Library INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/BSP
  ${CMAKE_CURRENT_SOURCE_DIR}/CPP_Library
)
target_link_libraries(GDUT_RC_Library INTERFACE
  STM32_Drivers
  ${TOOLCHAIN_LINK_LIBRARIES}
)
target_compile_definitions(GDUT_RC_Library INTERFACE
)

add_executable(test test/test.cpp)
target_link_libraries(test PRIVATE
  GDUT_RC_Library
  ${TOOLCHAIN_LINK_LIBRARIES}
)
