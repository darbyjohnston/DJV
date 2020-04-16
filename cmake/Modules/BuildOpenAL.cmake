include(ExternalProject)

set(OPENAL_CMAKE_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DBUILD_SHARED_LIBS=${OPENAL_SHARED_LIBS}
    -DALSOFT_UTILS=OFF
    -DALSOFT_EXAMPLES=OFF
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
if(NOT OPENAL_SHARED_LIBS)
    set(OPENAL_CMAKE_ARGS ${OPENAL_CMAKE_ARGS} -DLIBTYPE=STATIC)
endif()
ExternalProject_Add(
    OpenAL
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenAL
    URL http://github.com/kcat/openal-soft/archive/openal-soft-1.19.1.tar.gz
    CMAKE_ARGS ${OPENAL_CMAKE_ARGS})

