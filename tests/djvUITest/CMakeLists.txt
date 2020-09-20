set(header
    ActionGroupTest.h
    ButtonGroupTest.h
    EnumFuncTest.h
    WidgetTest.h)
set(source
    ActionGroupTest.cpp
    ButtonGroupTest.cpp
    EnumFuncTest.cpp
    WidgetTest.cpp)

add_library(djvUITest ${header} ${source})
target_link_libraries(djvUITest djvTestLib djvUI)
set_target_properties(
    djvUITest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)
