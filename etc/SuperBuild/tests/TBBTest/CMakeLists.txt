find_package(TBB REQUIRED)

set(source main.cpp)

add_executable(TBBTest ${header} ${source})
target_link_libraries(TBBTest TBB)

add_test(TBBTest TBBTest)
