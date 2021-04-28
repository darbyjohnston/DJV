set(header
    Cache.h
    CacheInline.h
    Core.h
    Enum.h
    Error.h
    ICommand.h
    ListObserver.h
    ListObserverInline.h
    MapObserver.h
    MapObserverInline.h
    Memory.h
    MemoryInline.h
    OS.h
    OSInline.h
    Observer.h
    Random.h
    RandomInline.h
    RapidJSON.h
    RapidJSONTemplates.h
    RapidJSONTemplatesInline.h
    StringFormat.h
    StringFormatInline.h
    String.h
    StringInline.h
    Time.h
    TimeInline.h
    UID.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h)
set(source
    Core.cpp
    Error.cpp
    ICommand.cpp
    Memory.cpp
    OS.cpp
    RapidJSON.cpp
    Random.cpp
    StringFormat.cpp
    String.cpp
    Time.cpp
    UID.cpp
    UndoStack.cpp)
if(WIN32)
    set(source
        ${source}
        ErrorWin32.cpp
        OSWin32.cpp
        TimeWin32.cpp)
else()
    set(source
        ${source}
        OSUnix.cpp
        TimeUnix.cpp)
endif()

add_library(djvCore ${header} ${source})
set(LIBRARIES
    RapidJSON)
if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm" AND NOT APPLE)
    set(LIBRARIES ${LIBRARIES} atomic)
endif()
target_link_libraries(djvCore ${LIBRARIES})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvCore
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvCore)

