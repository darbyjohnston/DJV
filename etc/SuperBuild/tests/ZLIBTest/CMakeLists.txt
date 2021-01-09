find_package(ZLIB REQUIRED)

set(source main.cpp)

add_executable(ZLIBTest ${header} ${source})
target_link_libraries(ZLIBTest ZLIB)

add_test(ZLIBTest ZLIBTest)
