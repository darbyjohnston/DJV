include(ExternalProject)

if(NOT DJV_THIRD_PARTY_DISABLE_BUILD)
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
        OpenALThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenAL
        URL http://github.com/kcat/openal-soft/archive/openal-soft-1.19.1.tar.gz
        CMAKE_ARGS ${OPENAL_CMAKE_ARGS})
endif()

set(OPENAL_FOUND TRUE)
if(NOT OPENAL_SHARED_LIBS)
    set(OPENAL_DEFINES AL_LIBTYPE_STATIC)
endif()
set(OPENAL_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/OpenAL32${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal${CMAKE_SHARED_LIBRARY_SUFFIX})
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
    add_dependencies(OpenAL::OpenAL OpenALThirdParty)
    set_target_properties(OpenAL::OpenAL PROPERTIES
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS "${OPENAL_DEFINES}")
endif()
if(OPENAL_FOUND AND NOT TARGET OpenAL)
    add_library(OpenAL INTERFACE)
    target_link_libraries(OpenAL INTERFACE OpenAL::OpenAL)
endif()

if(WIN32)
    install(
        FILES
        ${DJV_THIRD_PARTY}/bin/OpenAL32.dll
        DESTINATION ${DJV_INSTALL_BIN})
elseif(APPLE)
    install(
        FILES
        ${OPENAL_LIBRARY}
        ${DJV_THIRD_PARTY}/lib/libopenal.1.dylib
        ${DJV_THIRD_PARTY}/lib/libopenal.1.19.1.dylib
        DESTINATION ${DJV_INSTALL_LIB})
else()
    install(
        FILES
        ${OPENAL_LIBRARY}
        ${OPENAL_LIBRARY}.1
        ${OPENAL_LIBRARY}.1.19.1
        DESTINATION ${DJV_INSTALL_LIB})
endif()

