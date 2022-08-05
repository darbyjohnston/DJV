find_package(OpenColorIO REQUIRED)

set(source main.cpp)

add_executable(OCIOTest ${source})


find_package(OpenColorIO REQUIRED)
include(CMakePrintHelpers)
cmake_print_variables(OCIO_INCLUDE_DIRS)
include_directories (${OCIO_INCLUDE_DIRS})
target_link_libraries (OCIOTest ${OCIO_LIBRARIES})

add_test(OCIOTest OCIOTest)
