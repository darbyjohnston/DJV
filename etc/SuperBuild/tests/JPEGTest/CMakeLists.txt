find_package(JPEG REQUIRED)

set(source main.cpp)

add_executable(JPEGTest ${header} ${source})
target_link_libraries(JPEGTest JPEG)

add_test(JPEGTest JPEGTest)
