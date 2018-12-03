set(header
    FontSettings.h
    FontSettingsInline.h
    ISettings.h
    ISettingsInline.h
    SettingsSystem.h
    Style.h
    StyleInline.h
    StyleSettings.h
    System.h)
set(source
    FontSettings.cpp
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
