include(ExternalProject)

cmake(WARNING "From the OpenImageDenoise documentation: "
    "Please note that installing the Git LFS extension is required to correctly "
    "clone the repository. Cloning without Git LFS will seemingly succeed but "
    "actually some of the files will be invalid and thus compilation will fail.")

set(OpenImageDenoise_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DOIDN_STATIC_LIB=OFF
    -DOIDN_APPS=OFF)
if(CMAKE_CXX_STANDARD)
    set(OpenImageDenoise_ARGS ${OpenImageDenoise_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

ExternalProject_Add(
    OpenImageDenoise
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenImageDenoise
    DEPENDS TBB ISPC
    GIT_REPOSITORY https://github.com/OpenImageDenoise/oidn.git
    GIT_TAG v1.2.4
    GIT_SUBMODULES_RECURSE ON
    CMAKE_ARGS ${OpenImageDenoise_ARGS})
