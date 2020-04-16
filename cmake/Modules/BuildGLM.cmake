include(ExternalProject)

ExternalProject_Add(
    GLMThirdParty
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/GLM
    URL http://github.com/g-truc/glm/releases/download/0.9.9.5/glm-0.9.9.5.zip
    PATCH_COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/glm-patch/CMakeLists.txt
        ${CMAKE_CURRENT_BINARY_DIR}/GLM/src/GLMThirdParty/CMakeLists.txt
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})

