include(ExternalProject)

ExternalProject_Add(
    PicoJSON
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PicoJSON
    URL http://github.com/kazuho/picojson/archive/master.zip
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

