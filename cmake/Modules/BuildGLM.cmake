include(ExternalProject)

ExternalProject_Add(
    GLM
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/GLM
    URL http://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DGLM_TEST_ENABLE=FALSE
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/glm ${CMAKE_INSTALL_PREFIX}/include/glm)
