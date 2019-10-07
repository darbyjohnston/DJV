set(source
    main.cpp)

add_executable(djvTest ${header} ${source})
set(libraries)
if(NOT DJV_TINY_BUILD)
    set(libraries
        ${libraries}
        djvUITest
        djvAVTest)
endif()
set(libraries ${libraries} djvCoreTest)
target_link_libraries(djvTest ${libraries})
set_target_properties(
    djvTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

add_test(djvTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX})

if(DJV_GCOV)
    SETUP_TARGET_FOR_COVERAGE(
        djvTestCoverage
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/djvTest${CMAKE_EXECUTABLE_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/coverage)
endif(DJV_GCOV)

