cmake_minimum_required(VERSION 3.0)

project(gladGL2ES LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR})

add_library(glad glad/glad.h glad/glad.c)

install(
    TARGETS glad
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(FILES glad/glad.h DESTINATION include)
install(FILES KHR/khrplatform.h DESTINATION include/KHR)
