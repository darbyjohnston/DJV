set(header)

set(source
    djvTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvTest
#    ${CMAKE_SOURCE_DIR}/tests/djvViewLibTest
#    ${CMAKE_SOURCE_DIR}/tests/djvGuiTest
    ${CMAKE_SOURCE_DIR}/tests/djvImageIoPluginTest
    ${CMAKE_SOURCE_DIR}/tests/djvCoreTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvCoreIncludeDirs})

add_executable(djvTest ${header} ${source})

target_link_libraries(
    djvTest
#    djvViewLibTest
#    djvGuiTest
    djvImageIoPluginTest
    djvCoreTest
    djvTestLib
    ${djvCoreLibs})

set_target_properties(djvTest PROPERTIES FOLDER tests)

add_test(
    djvTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX})

if (djvGcov)

    SETUP_TARGET_FOR_COVERAGE(
        djvTestCoverage
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/coverage)

endif (djvGcov)

