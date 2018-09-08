set(header
    djvAssert.h
    djvBox.h
    djvBoxInline.h
    djvBox2Inline.h
    djvBox3Inline.h
    djvBoxUtil.h
    djvBoxUtilInline.h
    djvCoreContext.h
    djvCore.h
    djvDebug.h
    djvDebugInline.h
    djvDebugLog.h
    djvError.h
    djvErrorUtil.h
    djvFileInfo.h
    djvFileInfoInline.h
    djvFileInfoUtil.h
    djvFileIO.h
    djvFileIOInline.h
    djvFileIOUtil.h
    djvListUtil.h
    djvListUtilInline.h
    djvMath.h
    djvMathInline.h
    djvMatrix.h
    djvMatrixInline.h
    djvMemory.h
    djvMemoryInline.h
    djvPlugin.h
    djvRange.h
    djvRangeInline.h
    djvRangeUtil.h
    djvRangeUtilInline.h
    djvSequence.h
    djvSequenceInline.h
    djvSequenceUtil.h
    djvSequenceUtilInline.h
    djvSignalBlocker.h
    djvSpeed.h
    djvStringUtil.h
    djvStringUtilInline.h
    djvSystem.h
    djvTime.h
    djvTimer.h
    djvUser.h
    djvUtil.h
    djvVector.h
    djvVectorInline.h
    djvVectorUtil.h
    djvVectorUtilInline.h)
set(mocHeader
    djvCoreContext.h
    djvDebugLog.h
    djvFileInfo.h
    djvFileInfoUtil.h
    djvFileIO.h
    djvMemory.h
    djvPlugin.h
    djvSequence.h
    djvSpeed.h
    djvTime.h)
set(source
    djvAssert.cpp
    djvDebugLog.cpp
    djvCoreContext.cpp
    djvDebug.cpp
    djvError.cpp
    djvErrorUtil.cpp
    djvFileInfo.cpp
    djvFileInfoUtil.cpp
    djvFileIO.cpp
    djvFileIOUtil.cpp
    djvMath.cpp
    djvMemory.cpp
    djvPlugin.cpp
    djvSequence.cpp
    djvSequenceUtil.cpp
    djvSignalBlocker.cpp
    djvSpeed.cpp
    djvStringUtil.cpp
    djvSystem.cpp
    djvTime.cpp
    djvTimer.cpp
    djvUser.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_ADD_RESOURCES(rccSource djvCore.qrc)
QT5_CREATE_TRANSLATION(qmSource ${source}
    djvCore_en_US.ts
    djvCore_cs_CZ.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR})
add_library(djvCore ${header} ${source} ${mocSource} ${rccSource} ${qmSource})
target_link_libraries(djvCore
    ${djvThirdPartyOptional}
    Qt5
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(djvCore PROPERTIES FOLDER lib CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvCore PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${header}
    DESTINATION include)
install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

