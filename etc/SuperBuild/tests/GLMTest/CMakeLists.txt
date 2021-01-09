find_package(GLM REQUIRED)

set(source main.cpp)

add_executable(GLMTest ${header} ${source})
target_link_libraries(GLMTest GLM)

add_test(GLMTest GLMTest)
