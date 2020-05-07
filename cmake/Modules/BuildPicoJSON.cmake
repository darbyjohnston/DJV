include(ExternalProject)

# Tag: v1.3.0
set(PicoJSON_GIT_TAG 25fc213cca61ea22b3c2e4db8def9927562ba5f7)

ExternalProject_Add(
    PicoJSON
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PicoJSON
    GIT_REPOSITORY https://github.com/kazuho/picojson.git
    GIT_TAG ${PicoJSON_GIT_TAG}
    PATCH_COMMAND
        ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/picojson-patch/CMakeLists.txt
            ${CMAKE_CURRENT_BINARY_DIR}/PicoJSON/src/PicoJSON/CMakeLists.txt
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})

