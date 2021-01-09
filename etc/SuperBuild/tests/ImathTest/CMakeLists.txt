find_package(Imath REQUIRED)

set(source main.cpp)

add_executable(ImathTest ${header} ${source})
target_link_libraries(ImathTest Imath)

add_test(ImathTest ImathTest)
