find_package(glad REQUIRED)

set(source main.cpp)

add_executable(gladTest ${header} ${source})
target_link_libraries(gladTest glad)

add_test(gladTest gladTest)
