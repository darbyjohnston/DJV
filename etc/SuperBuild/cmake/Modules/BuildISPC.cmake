include(ExternalProject)

if (APPLE)
    set(ISPC_URL https://github.com/ispc/ispc/releases/download/v1.14.1/ispc-v1.14.1-macOS.tar.gz)
elseif(WIN32)
    set(ISPC_URL https://github.com/ispc/ispc/releases/download/v1.14.1/ispc-v1.14.1-windows.zip)
else()
    set(ISPC_URL https://github.com/ispc/ispc/releases/download/v1.14.1/ispc-v1.14.1-linux.tar.gz)
endif()

ExternalProject_Add(
    ISPC
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ISPC
    URL ${ISPC_URL}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        <SOURCE_DIR>/bin/ispc${CMAKE_EXECUTABLE_SUFFIX}
        ${CMAKE_INSTALL_PREFIX}/bin/ispc${CMAKE_EXECUTABLE_SUFFIX}
    BUILD_ALWAYS OFF)

set(ISPC_PATH "${CMAKE_INSTALL_PREFIX}/bin/ispc${CMAKE_EXECUTABLE_SUFFIX}")

#set(ISPC_ARGS
#    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
#    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#    -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib
#    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
#    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
#    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
#    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
#    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
#    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
#    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})
#if(CMAKE_CXX_STANDARD)
#    set(ISPC_ARGS ${ISPC_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
#endif()
#
#ExternalProject_Add(
#    ISPC
#    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ISPC
#    GIT_REPOSITORY "https://github.com/ispc/ispc.git"
#    GIT_TAG v1.14.1
#    CMAKE_ARGS ${ISPC_ARGS})

