cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0048 NEW)

project(
    FSeq
    VERSION 1.0
    DESCRIPTION "FSeq is a C library for working with numbered sequences of files."
    LANGUAGES C)

set(FSEQ_BUILD_BIN FALSE CACHE STRING "Whether to build the utilities")
set(FSEQ_BUILD_TESTS FALSE CACHE STRING "Whether to build the tests")

include_directories(${CMAKE_SOURCE_DIR})

add_library(fseq fseq.h fseq.c)

if(FSEQ_BUILD_BIN)
	add_subdirectory(bin)
endif()
if(FSEQ_BUILD_TESTS)
	add_subdirectory(tests)
endif()
