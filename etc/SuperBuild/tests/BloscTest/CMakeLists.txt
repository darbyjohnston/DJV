find_package(Blosc REQUIRED)

set(source main.cpp)

add_executable(BloscTest ${header} ${source})
target_link_libraries(BloscTest Blosc)

add_test(BloscTest BloscTest)
