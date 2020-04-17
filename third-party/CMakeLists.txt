cmake_minimum_required(VERSION 3.12)

project(DJV-third-party)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/../cmake/Modules)

set(DJV_THIRD_PARTY_OPTIONAL TRUE CACHE BOOL "Build optional dependencies")

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Select the GLAD OpenGL version.
if(DJV_OPENGL_ES2)
    include_directories(${CMAKE_SOURCE_DIR}/gladGL2ES)
    add_subdirectory(gladGL2ES)
else()
    include_directories(${CMAKE_SOURCE_DIR}/gladGL4_1)
    add_subdirectory(gladGL4_1)
endif()

# Add Python dependency.
if(DJV_PYTHON)
	find_package(Python)
	include(FetchContent)
	FetchContent_Declare(
		pybind11
		GIT_REPOSITORY https://github.com/pybind/pybind11
		GIT_TAG v2.3.0)
	FetchContent_GetProperties(pybind11)
	if(NOT pybind11_POPULATED)
		FetchContent_Populate(pybind11)
		add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
	endif()
endif()

# Set the list of required dependencies.
set(DJV_THIRD_PARTY_REQUIRED_DEPS ZLIB)
set(DJV_THIRD_PARTY_REQUIRED_DEPS
    ${DJV_THIRD_PARTY_REQUIRED_DEPS}
	FSeq
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

# Set the list of optional dependencies.
if(WIN32)
else()
    set(DJV_THIRD_PARTY_OPTIONAL_DEPS
        ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
		NASM
        #MbedTLS
        )
endif()
set(DJV_THIRD_PARTY_OPTIONAL_DEPS
    ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
    #curl
    FFmpeg
    JPEGTurbo
    TIFF
    OpenEXR)
if(WIN32)
    set(DJV_THIRD_PARTY_OPTIONAL_DEPS
        ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
        OpenNURBS)
elseif(APPLE)
    set(DJV_THIRD_PARTY_OPTIONAL_DEPS
        ${DJV_THIRD_PARTY_OPTIONAL_DEPS}
        OpenNURBS)
else()
endif()

# Set whether shared libraries are used.
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

# Build the dependencies.
foreach(DEP ${DJV_THIRD_PARTY_REQUIRED_DEPS})
	include(Build${DEP})
endforeach()
if(DJV_THIRD_PARTY_OPTIONAL)
	foreach(DEP ${DJV_THIRD_PARTY_OPTIONAL_DEPS})
		include(Build${DEP})
	endforeach()
endif()
