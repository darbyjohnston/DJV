include(ExternalProject)

ExternalProject_Add(
    PicoJSON_EXTERNAL
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PicoJSON
    URL http://github.com/kazuho/picojson/archive/master.zip
	PATCH_COMMAND
	    ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/third-party/picojson-patch/CMakeLists.txt ${CMAKE_CURRENT_BINARY_DIR}/PicoJSON/src/PicoJSON_EXTERNAL/CMakeLists.txt
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
        
set(PicoJSON_FOUND TRUE)
set(PicoJSON_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)

if(PicoJSON_FOUND AND NOT TARGET PicoJSON)
    add_library(PicoJSON INTERFACE)
    add_dependencies(PicoJSON PicoJSON_EXTERNAL)
    set_target_properties(PicoJSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PicoJSON_INCLUDE_DIRS}")
endif()

