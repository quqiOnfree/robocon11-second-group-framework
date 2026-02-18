cmake_minimum_required(VERSION 3.22)

enable_language(C CXX ASM)

project(GDUT_RC_Library)

add_library(GDUT_RC_Library
  ${CMAKE_CURRENT_SOURCE_DIR}/Experimental/bsp_can.cpp
)
target_include_directories(GDUT_RC_Library PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/BSP
  ${CMAKE_CURRENT_SOURCE_DIR}/CPP_Library
  ${CMAKE_CURRENT_SOURCE_DIR}/Experimental
)
target_link_libraries(GDUT_RC_Library PUBLIC
  STM32_Drivers
  ${TOOLCHAIN_LINK_LIBRARIES}
  FreeRTOS
  tlsf
)
target_compile_definitions(GDUT_RC_Library PUBLIC
)

# add_executable(test test/test.cpp)
# target_link_libraries(test PRIVATE
#   GDUT_RC_Library
# )
