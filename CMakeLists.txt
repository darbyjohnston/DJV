cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

set(DJV_VERSION_MAJOR "2")
set(DJV_VERSION_MINOR "0")
set(DJV_VERSION_PATCH "4")
set(DJV_VERSION ${DJV_VERSION_MAJOR}.${DJV_VERSION_MINOR}.${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION_MAJOR=${DJV_VERSION_MAJOR})
add_definitions(-DDJV_VERSION_MINOR=${DJV_VERSION_MINOR})
add_definitions(-DDJV_VERSION_PATCH=${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION="${DJV_VERSION}")

project(
    DJV
    VERSION ${DJV_VERSION}
    DESCRIPTION "Professional media review software for VFX, animation, and film production"
    HOMEPAGE_URL "http://djv.sourceforge.net"
    LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------

# Set the platform we are building for.
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

# Update the CMake module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

# Set the output directories for the build.
set(DJV_BUILD_DIR ${PROJECT_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/bin)

# Set the installation directories.
set(DJV_INSTALL_BIN bin)
set(DJV_INSTALL_LIB lib)
set(DJV_INSTALL_PLUGINS plugins)
set(DJV_INSTALL_DOC doc)
set(DJV_INSTALL_TRANSLATIONS translations)

# Set miscellaneous options.
#add_definitions(-DDJV_MMAP)
#add_definitions(-DDJV_OPENGL_PBO)
add_definitions(-DDJV_ASSERT)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set whether to use OpenGL ES.
set(DJV_OPENGL_ES2 FALSE CACHE STRING "Whether to enable OpenGL ES")
if(DJV_OPENGL_ES2)
    add_definitions(-DDJV_OPENGL_ES2)
endif()

# Enable testing.
enable_testing()

# Enable gcov code coverage targets.
set(DJV_GCOV false)

# Set compiler specific options.
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX)
        # Disable warnings from XPM files.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
        
        # Disable warnings from printf/sprintf.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

        # Set profiling options.    
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

        # Set gcov options.
        if(DJV_GCOV)
            include(CodeCoverage)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
        endif()
    endif()
endif()

# Include directories.
include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/bin
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/third-party)

#-------------------------------------------------------------------------------
# Third Party Libraries
#-------------------------------------------------------------------------------

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(third-party/fseq)

if(DJV_OPENGL_ES2)
    include_directories(${CMAKE_SOURCE_DIR}/third-party/gladGL2ES)
    add_subdirectory(third-party/gladGL2ES)
else()
    include_directories(${CMAKE_SOURCE_DIR}/third-party/gladGL4_1)
    add_subdirectory(third-party/gladGL4_1)
endif()

if(DJV_THIRD_PARTY)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

    set(ZLIB_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(OPENAL_SHARED_LIBS ON)
    set(JPEG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(TIFF_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(PNG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(ILMBASE_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(OPENEXR_SHARED_LIBS ${BUILD_SHARED_LIBS})
	set(OCIO_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(FFMPEG_SHARED_LIBS ON)
else()
    set(ZLIB_SHARED_LIBS ON)
    set(OPENAL_SHARED_LIBS ON)
    set(JPEG_SHARED_LIBS ON)
    set(TIFF_SHARED_LIBS ON)
    set(PNG_SHARED_LIBS ON)
    set(ILMBASE_SHARED_LIBS ON)
    set(OPENEXR_SHARED_LIBS ON)
	set(OCIO_SHARED_LIBS ON)
    set(FFMPEG_SHARED_LIBS ON)
endif()

set(DJV_THIRD_PARTY_PACKAGES_REQUIRED
    ZLIB
    #curl
    GLM
    GLFW
    PicoJSON
    OpenAL
    IlmBase
    OCIO
    FreeType)
set(DJV_THIRD_PARTY_PACKAGES_OPTIONAL
    JPEGTurbo
    TIFF
    PNG
    OpenEXR)
if(WIN32)
    # See the directions for building FFmpeg on Windows in "doc/BuildWindows.html".
elseif(APPLE)
    set(DJV_THIRD_PARTY_PACKAGES_OPTIONAL
        ${DJV_THIRD_PARTY_PACKAGES_OPTIONAL}
        FFmpeg)
else()
    set(DJV_THIRD_PARTY_PACKAGES_OPTIONAL
        ${DJV_THIRD_PARTY_PACKAGES_OPTIONAL}
        FFmpeg
        #MbedTLS
        )
endif()

if(DJV_THIRD_PARTY)
    foreach(PACKAGE ${DJV_THIRD_PARTY_PACKAGES_REQUIRED})
        include(Build${PACKAGE})
    endforeach()
    foreach(PACKAGE ${DJV_THIRD_PARTY_PACKAGES_OPTIONAL})
        include(Build${PACKAGE})
    endforeach()
else()
    foreach(PACKAGE ${DJV_THIRD_PARTY_PACKAGES_REQUIRED})
        find_package(${PACKAGE} REQUIRED)
    endforeach()
    foreach(PACKAGE ${DJV_THIRD_PARTY_PACKAGES_OPTIONAL})
        find_package(${PACKAGE})
    endforeach()
endif()

#-------------------------------------------------------------------------------
# Resources
#-------------------------------------------------------------------------------

file(GLOB_RECURSE DJV_TEXT_FILES ${CMAKE_SOURCE_DIR}/etc/Text/*.text)
file(COPY ${DJV_TEXT_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Text)
install(FILES ${DJV_TEXT_FILES} DESTINATION etc/Text)

file(GLOB_RECURSE DJV_FONT_FILES ${CMAKE_SOURCE_DIR}/etc/Fonts/*.ttf ${CMAKE_SOURCE_DIR}/etc/Fonts/*.otf)
file(COPY ${DJV_FONT_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Fonts)
install(FILES ${DJV_FONT_FILES} DESTINATION etc/Fonts)

file(GLOB DJV_ICON_FILES
	${CMAKE_SOURCE_DIR}/etc/Icons/*.ppm
	${CMAKE_SOURCE_DIR}/etc/Icons/*.png)
file(COPY ${DJV_ICON_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Icons)
install(FILES ${DJV_ICON_FILES} DESTINATION etc/Icons)
foreach(DPI 96 120 144 168 192 216 240 264 288)
    file(GLOB_RECURSE DJV_ICON_FILES
        ${CMAKE_SOURCE_DIR}/etc/Icons/${DPI}DPI/*.ppm
        ${CMAKE_SOURCE_DIR}/etc/Icons/${DPI}DPI/*.png)
    file(COPY ${DJV_ICON_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Icons/${DPI}DPI)
    install(FILES ${DJV_ICON_FILES} DESTINATION etc/Icons/${DPI}DPI)
endforeach()

file(GLOB_RECURSE DJV_IMAGE_FILES
    ${CMAKE_SOURCE_DIR}/etc/Images/*.ppm
    ${CMAKE_SOURCE_DIR}/etc/Images/*.png)
file(COPY ${DJV_IMAGE_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Images)
install(FILES ${DJV_IMAGE_FILES} DESTINATION etc/Images)

file(GLOB_RECURSE DJV_MODEL_FILES
    ${CMAKE_SOURCE_DIR}/etc/Models/*.obj
    ${CMAKE_SOURCE_DIR}/etc/Models/*.3dm)
file(COPY ${DJV_MODEL_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Models)
install(FILES ${DJV_MODEL_FILES} DESTINATION etc/Models)

file(GLOB_RECURSE DJV_AUDIO_FILES ${CMAKE_SOURCE_DIR}/etc/Audio/*.wav)
file(COPY ${DJV_AUDIO_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Audio)
install(FILES ${DJV_AUDIO_FILES} DESTINATION etc/Audio)

if(DJV_OPENGL_ES2)
    file(GLOB_RECURSE DJV_SHADER_FILES ${CMAKE_SOURCE_DIR}/etc/Shaders/GL2ES/*.glsl)
else()
    file(GLOB_RECURSE DJV_SHADER_FILES ${CMAKE_SOURCE_DIR}/etc/Shaders/GL4_1/*.glsl)
endif()
file(COPY ${DJV_SHADER_FILES} DESTINATION ${DJV_BUILD_DIR}/etc/Shaders)
install(FILES ${DJV_SHADER_FILES} DESTINATION etc/Shaders)

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
    ${DJV_SHADER_FILES})

#-------------------------------------------------------------------------------
# Subdirectories
#-------------------------------------------------------------------------------

include(djvPackage)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(experiments)
add_subdirectory(doc)

include(CPack)

