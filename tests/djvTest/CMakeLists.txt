set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(djvTest ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvTest djvCoreTest)
set_target_properties(
    djvTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

add_test(
    djvTest
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX})

if (DJV_GCOV)
    SETUP_TARGET_FOR_COVERAGE(
        djvTestCoverage
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/coverage)
endif (DJV_GCOV)

