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
    OpenAL_EXTERNAL
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenAL
    URL http://github.com/kcat/openal-soft/archive/openal-soft-1.19.1.tar.gz
    CMAKE_ARGS ${OPENAL_CMAKE_ARGS})

set(OPENAL_FOUND TRUE)
if(NOT OPENAL_SHARED_LIBS)
    set(OPENAL_DEFINES AL_LIBTYPE_STATIC)
endif()
set(OPENAL_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    if(WIN32)
    elseif(APPLE)
    else()
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.so)
    endif()
else()
    if(WIN32)
    elseif(APPLE)
    else()
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.so)
    endif()
endif()
if(WIN32)
    set(OPENAL_LIBRARIES ${OPENAL_LIBRARY} Winmm)
elseif(APPLE)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(CORE_AUDIO CoreAudio)
    set(OPENAL_LIBRARIES
        ${OPENAL_LIBRARY}
        ${AUDIO_TOOLBOX}
        ${CORE_AUDIO})
else()
    set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})
endif()

if(OPENAL_FOUND AND NOT TARGET OpenAL::OpenAL)
    add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
    add_dependencies(OpenAL::OpenAL OpenAL_EXTERNAL)
    set_target_properties(OpenAL::OpenAL PROPERTIES
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS "${OPENAL_DEFINES}")
endif()
if(OPENAL_FOUND AND NOT TARGET OpenAL)
    add_library(OpenAL INTERFACE)
    target_link_libraries(OpenAL INTERFACE OpenAL::OpenAL)
endif()

