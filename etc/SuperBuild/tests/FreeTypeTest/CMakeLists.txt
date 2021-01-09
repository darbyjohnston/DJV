find_package(FreeType REQUIRED)

set(source main.cpp)

add_executable(FreeTypeTest ${header} ${source})
target_link_libraries(FreeTypeTest FreeType)

add_test(FreeTypeTest FreeTypeTest)
