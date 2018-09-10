set(header)
set(source
    djvTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvTest
    ${CMAKE_SOURCE_DIR}/tests/djvViewLibTest
    ${CMAKE_SOURCE_DIR}/tests/djvUITest
    ${CMAKE_SOURCE_DIR}/tests/djvGraphicsTest
    ${CMAKE_SOURCE_DIR}/tests/djvCoreTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${OPENGL_INCLUDE_DIRS})

add_executable(djvTest ${header} ${source})
target_link_libraries(
    djvTest
    djvViewLibTest
    djvUITest
    djvGraphicsTest
    djvCoreTest)
set_target_properties(djvTest PROPERTIES FOLDER tests CXX_STANDARD 11)

add_test(
    djvTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX})

if (djvGcov)
    SETUP_TARGET_FOR_COVERAGE(
        djvTestCoverage
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/coverage)
endif (djvGcov)

