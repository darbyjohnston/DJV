include(ExternalProject)

ExternalProject_Add(
    GLFW_EXTERNAL
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/GLFW
    URL http://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.zip
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DGLFW_SHARED_LIBS=${GLFW_SHARED_LIBS})

set(GLFW_FOUND TRUE)
set(GLFW_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    if(WIN32)
    elseif(APPLE)
    else()
        set(GLFW_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglfw3.a)
    endif()
else()
    if(WIN32)
    elseif(APPLE)
    else()
        set(GLFW_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglfw3.a)
    endif()
endif()
set(GLFW_LIBRARIES ${GLFW_LIBRARY})

if(DJV_PLATFORM_OSX)
    find_library(CORE_VIDEO CoreVideo REQUIRED)
    find_library(IO_KIT IOKit REQUIRED)
    find_library(COCOA Cocoa REQUIRED)
    find_library(CARBON Carbon REQUIRED)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${CORE_VIDEO} ${IO_KIT} ${COCOA} ${CARBON})
elseif(DJV_PLATFORM_LINUX)
    find_package(X11 REQUIRED)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${X11_LIBRARIES})
    set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
endif()

if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
    add_library(GLFW::GLFW UNKNOWN IMPORTED)
    add_dependencies(GLFW::GLFW GLFW_EXTERNAL)
    set_target_properties(GLFW::GLFW PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS GLFW_FOUND)
endif()
if(GLFW_FOUND AND NOT TARGET GLFW)
    add_library(GLFW INTERFACE)
    target_link_libraries(GLFW INTERFACE GLFW::GLFW)
    if(DJV_PLATFORM_OSX)
        target_link_libraries(GLFW INTERFACE ${CORE_VIDEO} ${IO_KIT} ${COCOA} ${CARBON})
    elseif (DJV_PLATFORM_LINUX)
        target_link_libraries(GLFW INTERFACE ${X11_LIBRARIES})
    endif()
endif()

