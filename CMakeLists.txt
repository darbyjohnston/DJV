cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

set(DJV_VERSION_MAJOR "2")
set(DJV_VERSION_MINOR "0")
set(DJV_VERSION_PATCH "6")
set(DJV_VERSION ${DJV_VERSION_MAJOR}.${DJV_VERSION_MINOR}.${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION_MAJOR=${DJV_VERSION_MAJOR})
add_definitions(-DDJV_VERSION_MINOR=${DJV_VERSION_MINOR})
add_definitions(-DDJV_VERSION_PATCH=${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION="${DJV_VERSION}")

project(
    DJV
    VERSION ${DJV_VERSION}
    DESCRIPTION "Professional review software for VFX, animation, and film production"
    HOMEPAGE_URL "http://djv.sourceforge.net"
    LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------

# General options.
set(DJV_BUILD_BIN TRUE CACHE BOOL "Build binaries")
set(DJV_BUILD_DOCS TRUE CACHE BOOL "Build documentation")
set(DJV_BUILD_EXAMPLES TRUE CACHE BOOL "Build examples")
set(DJV_BUILD_TINY FALSE CACHE BOOL "Enable a 'tiny' build (only build core libraries + tests)")
if(DJV_BUILD_TINY)
    add_definitions(-DDJV_BUILD_TINY)
endif()

# Test options.
enable_testing()
set(DJV_GPROF FALSE CACHE BOOL "Enable gprof profiling")
set(DJV_GCOV FALSE CACHE BOOL "Enable gcov code coverage")

# Third party options.
set(DJV_THIRD_PARTY TRUE CACHE BOOL "Build third party dependencies")
set(DJV_THIRD_PARTY_OPTIONAL TRUE CACHE BOOL "Build optional third party dependencies")

# This is used to disable ExternalProject_Add() for third party dependencies. This
# can be helpful for speeding up compile iterations after the third party
# dependencies have been built.
set(DJV_THIRD_PARTY_DISABLE_BUILD FALSE CACHE BOOL "Disable building third party dependencies")

# Experimental options.
set(DJV_OPENGL_ES2 FALSE CACHE BOOL "OpenGL ES 2.0 rendering (experiemental)")
if(DJV_OPENGL_ES2)
    add_definitions(-DDJV_OPENGL_ES2)
endif()
set(DJV_PYTHON FALSE CACHE BOOL "Python support (experimental)")
if(DJV_PYTHON)
    add_definitions(-DDJV_PYTHON)
endif()

#-------------------------------------------------------------------------------
# Configuration
#-------------------------------------------------------------------------------

# Setup the platform we are building for.
if(APPLE)
    set(DJV_PLATFORM_OSX TRUE)
    add_definitions(-DDJV_PLATFORM_OSX)
elseif(UNIX)
    string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} Linux _TMP)
    if(_TMP)
        set(DJV_PLATFORM_LINUX TRUE)
        add_definitions(-DDJV_PLATFORM_LINUX)
        set(CMAKE_C_FLAGS -fPIC)
        set(CMAKE_CXX_FLAGS -fPIC)
    endif()
elseif(WIN32)
    set(DJV_PLATFORM_WINDOWS TRUE)
    add_definitions(-DDJV_PLATFORM_WINDOWS)
	set(CMAKE_C_FLAGS "/MP /EHsc")
	set(CMAKE_CXX_FLAGS "/MP /EHsc")
endif()

# Set the output directories for the build.
set(DJV_BUILD_DIR ${PROJECT_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/bin)

# Set the installation directories.
set(DJV_INSTALL_BIN bin)
set(DJV_INSTALL_LIB lib)
set(DJV_INSTALL_DOCS docs)

# set the include directories.
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/bin
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/third-party)
if(DJV_PYTHON)
    set(INCLUDE_DIRS
        ${INCLUDE_DIRS}
        ${pybind11_SOURCE_DIR}/include)
endif()
include_directories(${INCLUDE_DIRS})

# Miscellaneous settings.
#add_definitions(-DDJV_MMAP)
#add_definitions(-DDJV_OPENGL_PBO)
add_definitions(-DDJV_ASSERT)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(DJV_DEMO)
    add_definitions(-DDJV_DEMO)
endif()
if(DJV_DEMO_THREADS)
    add_definitions(-DDJV_DEMO_THREADS)
endif()

# Test settings.
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX)
        if(DJV_GPROF)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
        endif()
        if(DJV_GCOV)
            include(CodeCoverage)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
        endif()
    endif()
endif()

#-------------------------------------------------------------------------------
# Third Party Dependencies
#-------------------------------------------------------------------------------

# Add threads and OpenGL dependencies.
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Select the GLAD OpenGL version.
if(DJV_OPENGL_ES2)
    include_directories(${CMAKE_SOURCE_DIR}/third-party/gladGL2ES)
    add_subdirectory(third-party/gladGL2ES)
else()
    include_directories(${CMAKE_SOURCE_DIR}/third-party/gladGL4_1)
    add_subdirectory(third-party/gladGL4_1)
endif()

# Add Python dependency.
if(DJV_PYTHON)
	find_package(Python)
endif()

# Add FSeq dependency.
add_subdirectory(third-party/fseq)

# If we are building third party dependencies setup the install paths.
if(DJV_THIRD_PARTY)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
    set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/install)
endif()

# Set the list of required third party dependencies.
set(DJV_THIRD_PARTY_REQUIRED_DEPS ZLIB)
set(DJV_THIRD_PARTY_REQUIRED_DEPS
    ${DJV_THIRD_PARTY_REQUIRED_DEPS}
    GLM
    GLFW
    PicoJSON
    #OpenAL
    RtAudio
    IlmBase
    PNG
    OCIO
    #OTIO
    FreeType)

# Set the list of optional third party dependencies.
if(WIN32)
else()
    set(DJV_THIRD_PARTY_OPTIONAL_DEPS
        ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
        #MbedTLS
        )
endif()
set(DJV_THIRD_PARTY_OPTIONAL_DEPS
    ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
    #curl
    #FFmpeg
    JPEGTurbo
    TIFF
    OpenEXR)
if(WIN32)
    set(DJV_THIRD_PARTY_OPTIONAL_DEPS
        ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
        OpenNURBS)
else()
endif()

# Set whether shared libraries are used for dependencies.
if(DJV_THIRD_PARTY)
    set(ZLIB_SHARED_LIBS ${BUILD_SHARED_LIBS})
    #set(OPENAL_SHARED_LIBS ON)
    set(RtAudio_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(JPEG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(TIFF_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(PNG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(ILMBASE_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(OPENEXR_SHARED_LIBS ${BUILD_SHARED_LIBS})
	set(OCIO_SHARED_LIBS ${BUILD_SHARED_LIBS})
	set(OTIO_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(FFmpeg_SHARED_LIBS ON)
else()
    set(ZLIB_SHARED_LIBS ON)
    #set(OPENAL_SHARED_LIBS ON)
    set(RtAudio_SHARED_LIBS ON)
    set(JPEG_SHARED_LIBS ON)
    set(TIFF_SHARED_LIBS ON)
    set(PNG_SHARED_LIBS ON)
    set(ILMBASE_SHARED_LIBS ON)
    set(OPENEXR_SHARED_LIBS ON)
	set(OCIO_SHARED_LIBS ON)
	set(OTIO_SHARED_LIBS ON)
    set(FFmpeg_SHARED_LIBS ON)
endif()

# Add the third party dependencies.
if(DJV_THIRD_PARTY)
    foreach(PACKAGE ${DJV_THIRD_PARTY_REQUIRED_DEPS})
        include(Build${PACKAGE})
    endforeach()
    if(DJV_THIRD_PARTY_OPTIONAL)
        foreach(PACKAGE ${DJV_THIRD_PARTY_OPTIONAL_DEPS})
            include(Build${PACKAGE})
        endforeach()
    endif()
else()
    foreach(PACKAGE ${DJV_THIRD_PARTY_REQUIRED_DEPS})
        find_package(${PACKAGE} REQUIRED)
    endforeach()
    foreach(PACKAGE ${DJV_THIRD_PARTY_OPTIONAL_DEPS})
        find_package(${PACKAGE})
    endforeach()
endif()

#-------------------------------------------------------------------------------
# Resources
#-------------------------------------------------------------------------------

# Text files (translations).
file(GLOB_RECURSE DJV_TEXT_FILES ${CMAKE_SOURCE_DIR}/etc/Text/*.text)
file(COPY ${DJV_TEXT_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Text)
install(FILES ${DJV_TEXT_FILES} DESTINATION etc/Text)

# Fonts.
file(GLOB_RECURSE DJV_FONT_FILES ${CMAKE_SOURCE_DIR}/etc/Fonts/*.ttf ${CMAKE_SOURCE_DIR}/etc/Fonts/*.otf)
file(COPY ${DJV_FONT_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Fonts)
install(FILES ${DJV_FONT_FILES} DESTINATION etc/Fonts)

# Icons.
file(GLOB DJV_ICON_FILES
	${CMAKE_SOURCE_DIR}/etc/Icons/*.ppm
	${CMAKE_SOURCE_DIR}/etc/Icons/*.png)
file(COPY ${DJV_ICON_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Icons)
install(FILES ${DJV_ICON_FILES} DESTINATION etc/Icons)
foreach(DPI 32 48 64 96 120 144 168 192 216 240 264 288)
    file(GLOB_RECURSE DJV_ICON_FILES
        ${CMAKE_SOURCE_DIR}/etc/Icons/${DPI}DPI/*.ppm
        ${CMAKE_SOURCE_DIR}/etc/Icons/${DPI}DPI/*.png)
    file(COPY ${DJV_ICON_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Icons/${DPI}DPI)
    install(FILES ${DJV_ICON_FILES} DESTINATION etc/Icons/${DPI}DPI)
endforeach()

# Images.
file(GLOB_RECURSE DJV_IMAGE_FILES
    ${CMAKE_SOURCE_DIR}/etc/Images/*.ppm
    ${CMAKE_SOURCE_DIR}/etc/Images/*.png)
file(COPY ${DJV_IMAGE_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Images)
install(FILES ${DJV_IMAGE_FILES} DESTINATION etc/Images)

# Models.
file(GLOB_RECURSE DJV_MODEL_FILES
    ${CMAKE_SOURCE_DIR}/etc/Models/*.obj
    ${CMAKE_SOURCE_DIR}/etc/Models/*.3dm)
file(COPY ${DJV_MODEL_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Models)
install(FILES ${DJV_MODEL_FILES} DESTINATION etc/Models)

# Audio.
file(GLOB_RECURSE DJV_AUDIO_FILES ${CMAKE_SOURCE_DIR}/etc/Audio/*.wav)
file(COPY ${DJV_AUDIO_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Audio)
install(FILES ${DJV_AUDIO_FILES} DESTINATION etc/Audio)

# Shaders.
if(DJV_OPENGL_ES2)
    file(GLOB_RECURSE DJV_SHADER_FILES ${CMAKE_SOURCE_DIR}/etc/Shaders/GL2ES/*.glsl)
else()
    file(GLOB_RECURSE DJV_SHADER_FILES ${CMAKE_SOURCE_DIR}/etc/Shaders/GL4_1/*.glsl)
endif()
file(COPY ${DJV_SHADER_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Shaders)
install(FILES ${DJV_SHADER_FILES} DESTINATION etc/Shaders)

# Color.
file(GLOB DJV_COLOR_DIRS ${CMAKE_SOURCE_DIR}/etc/Color/*)
foreach(DIR ${DJV_COLOR_DIRS})
    file(COPY ${DIR} DESTINATION ${DJV_BUILD_DIR}/etc/Color)
    install(DIRECTORY ${DIR} DESTINATION etc/Color)
endforeach()

#function(add_vulkan_vert_shader TARGET ARG)
#    add_custom_command(
#        OUTPUT ${DJV_SHADERS_DIR}/${ARG}.spv
#        COMMAND ${CMAKE_COMMAND} -E make_directory ${DJV_SHADERS_DIR}
#        COMMAND ${Vulkan_SHADER_COMPILER} -V ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}.vert -o ${DJV_SHADERS_DIR}/${ARG}.spv
#        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}.vert)
#    add_custom_target(${ARG} DEPENDS ${DJV_SHADERS_DIR}/${ARG}.spv)
#    set_target_properties(${ARG} PROPERTIES FOLDER "Shaders")
#    add_dependencies(${TARGET} ${ARG})
#    install(FILES ${DJV_SHADERS_DIR}/${ARG}.spv DESTINATION etc/Shaders)
#endfunction()
#
#function(add_vulkan_frag_shader TARGET ARG)
#    add_custom_command(
#        OUTPUT ${DJV_SHADERS_DIR}/${ARG}.spv
#        COMMAND ${CMAKE_COMMAND} -E make_directory ${DJV_SHADERS_DIR}
#        COMMAND ${Vulkan_SHADER_COMPILER} -V ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}.frag -o ${DJV_SHADERS_DIR}/${ARG}.spv
#        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}.frag)
#    add_custom_target(${ARG} DEPENDS ${DJV_SHADERS_DIR}/${ARG}.spv)
#    set_target_properties(${ARG} PROPERTIES FOLDER "Shaders")
#    add_dependencies(${TARGET} ${ARG})
#    install(FILES ${DJV_SHADERS_DIR}/${ARG}.spv DESTINATION etc/Shaders)
#endfunction()

set(DJV_RESOURCE_FILES
    ${DJV_TEXT_FILES}
    ${DJV_FONT_FILES}
    ${DJV_ICON_FILES}
    ${DJV_IMAGE_FILES}
    ${DJV_MODEL_FILES}
    ${DJV_AUDIO_FILES}
    ${DJV_SHADER_FILES}
    ${DJV_COLOR_DIRS})

#-------------------------------------------------------------------------------
# Subdirectories
#-------------------------------------------------------------------------------

include(djvPackage)

add_subdirectory(third-party)
add_subdirectory(lib)
add_subdirectory(tests)
if(NOT DJV_BUILD_TINY)
    if(DJV_BUILD_BIN)
        add_subdirectory(bin)
    endif()
    if(DJV_BUILD_DOCS)
        add_subdirectory(docs)
    endif()
    if(DJV_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

include(CPack)
