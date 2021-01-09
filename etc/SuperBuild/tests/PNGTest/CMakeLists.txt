find_package(PNG REQUIRED)

set(source main.cpp)

add_executable(PNGTest ${header} ${source})
target_link_libraries(PNGTest PNG)

add_test(PNGTest PNGTest)
