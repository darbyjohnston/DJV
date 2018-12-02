set(header
    ISettings.h
    ISettingsInline.h
    SettingsSystem.h
    Style.h
    StyleInline.h
    StyleSettings.h
    System.h)
set(source
    ISettings.cpp
    SettingsSystem.cpp
    Style.cpp
    StyleSettings.cpp
    System.cpp)

add_library(djvUICore ${header} ${source})
target_link_libraries(djvUICore djvAV)
set_target_properties(
    djvUICore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
