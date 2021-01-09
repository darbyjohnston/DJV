find_package(Boost REQUIRED)

set(source main.cpp)

add_executable(BoostTest ${header} ${source})
target_link_libraries(BoostTest Boost::boost)

add_test(BoostTest BoostTest)
