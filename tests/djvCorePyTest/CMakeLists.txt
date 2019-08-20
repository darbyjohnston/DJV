set(tests
    AnimationTest
    BBoxTest
    ContextTest
    PathTest
    FileInfoTest
    FrameTest
    VectorTest)
foreach(test ${tests})
    file(COPY ${test}.py DESTINATION ${DJV_BUILD_DIR}/bin)
    add_test(NAME ${test}Py
        COMMAND ${PYTHON_EXECUTABLE} ${DJV_BUILD_DIR}/bin/${test}.py
        WORKING_DIRECTORY $<TARGET_FILE_DIR:djvCorePy>)
endforeach()
