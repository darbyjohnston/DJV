set(header
    EnumTest.h
    WidgetTest.h)
set(source
    EnumTest.cpp
    WidgetTest.cpp)

add_library(djvUITest ${header} ${source})
target_link_libraries(djvUITest djvTestLib djvUI)
set_target_properties(
    djvUITest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)
