include(ExternalProject)

ExternalProject_Add(
    TIFF
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/TIFF
    DEPENDS ZLIB JPEG
    URL http://download.osgeo.org/libtiff/tiff-4.0.10.tar.gz
    PATCH_COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tiff-patch/CMakeLists.txt
        ${CMAKE_CURRENT_BINARY_DIR}/TIFF/src/TIFF/CMakeLists.txt
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DBUILD_SHARED_LIBS=${TIFF_SHARED_LIBS}
        -DZLIB_SHARED_LIBS=${ZLIB_SHARED_LIBS}
        -DJPEG_SHARED_LIBS=${JPEG_SHARED_LIBS}
        -Djbig=OFF
        -Dlzma=OFF
        -Dzstd=OFF
        -Dwebp=OFF
        -Djpeg12=OFF)

