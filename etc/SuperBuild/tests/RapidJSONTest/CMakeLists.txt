find_package(RapidJSON REQUIRED)

set(source main.cpp)

add_executable(RapidJSONTest ${header} ${source})
target_link_libraries(RapidJSONTest RapidJSON)

add_test(RapidJSONTest RapidJSONTest)
