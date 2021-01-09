find_package(embree REQUIRED)

set(source main.cpp)

add_executable(embreeTest ${header} ${source})
target_link_libraries(embreeTest embree)

add_test(embreeTest embreeTest)
