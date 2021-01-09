find_package(IlmBase REQUIRED)

set(source main.cpp)

add_executable(IlmBaseTest ${header} ${source})
target_link_libraries(IlmBaseTest IlmBase)

add_test(IlmBaseTest IlmBaseTest)
