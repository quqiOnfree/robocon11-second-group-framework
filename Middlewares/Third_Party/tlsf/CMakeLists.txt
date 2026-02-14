cmake_minimum_required(VERSION 3.24)

add_library(tlsf STATIC
  tlsf.c
  tlsf.h
)
target_include_directories(tlsf PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
