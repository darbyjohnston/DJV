set(header
    djvAssert.h
    djvBox.h
    djvBox2Inline.h
    djvBox3Inline.h
    djvBoxInline.h
    djvBoxUtil.h
    djvBoxUtilInline.h
    djvCglContextPrivate.h
    djvColor.h
    djvColorInline.h
    djvColorUtil.h
    djvColorUtilInline.h
    djvColorProfile.h
    djvCoreApplication.h
    djvCoreExport.h
    djvDebug.h
    djvDebugInline.h
    djvDebugLog.h
    djvError.h
    djvErrorUtil.h
    djvFileInfo.h
    djvFileInfoInline.h
    djvFileInfoUtil.h
    djvFileIo.h
    djvFileIoInline.h
    djvFileIoUtil.h
    djvGlxContextPrivate.h
    djvImage.h
    djvImageApplication.h
    djvImageIo.h
    djvImageTags.h
    djvImageUtil.h
    djvListUtil.h
    djvListUtilInline.h
    djvMath.h
    djvMathInline.h
    djvMatrix.h
    djvMatrix3Inline.h
    djvMatrix4Inline.h
    djvMatrixInline.h
    djvMatrixUtil.h
    djvMatrixUtilInline.h
    djvMemory.h
    djvMemoryBuffer.h
    djvMemoryBufferInline.h
    djvMemoryInline.h
    djvOpenGl.h
    djvOpenGlContext.h
    djvOpenGlImage.h
    djvOpenGlOffscreenBuffer.h
    djvPixel.h
    djvPixelData.h
    djvPixelDataInline.h
    djvPixelDataUtil.h
    djvPixelInline.h
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
    djvVector2Inline.h
    djvVector3Inline.h
    djvVectorInline.h
    djvVectorUtil.h
    djvVectorUtilInline.h
    djvWglContextPrivate.h)

set(mocHeader
    djvColorProfile.h
    djvCoreApplication.h
    djvDebugLog.h
    djvFileInfo.h
    djvFileInfoUtil.h
    djvFileIo.h
    djvImageApplication.h
    djvImageIo.h
    djvImageTags.h
    djvMemory.h
    djvOpenGlImage.h
    djvPixelData.h
    djvPixel.h
    djvPlugin.h
    djvSequence.h
    djvSpeed.h
    djvTime.h)

set(source
    djvAssert.cpp
    djvDebugLog.cpp
    djvCglContextPrivate.cpp
    djvColor.cpp
    djvColorUtil.cpp
    djvColorProfile.cpp
    djvCoreApplication.cpp
    djvDebug.cpp
    djvError.cpp
    djvErrorUtil.cpp
    djvFileInfo.cpp
    djvFileInfoUtil.cpp
    djvFileIo.cpp
    djvFileIoUtil.cpp
    djvGlxContextPrivate.cpp
    djvImage.cpp
    djvImageApplication.cpp
    djvImageIo.cpp
    djvImageTags.cpp
    djvImageUtil.cpp
    djvMath.cpp
    djvMemory.cpp
    djvOpenGl.cpp
    djvOpenGlContext.cpp
    djvOpenGlImage.cpp
    djvOpenGlImageDraw.cpp
    djvOpenGlOffscreenBuffer.cpp
    djvPixel.cpp
    djvPixelConvert.cpp
    djvPixelData.cpp
    djvPixelDataUtil.cpp
    djvPlugin.cpp
    djvSequence.cpp
    djvSequenceUtil.cpp
    djvSignalBlocker.cpp
    djvSpeed.cpp
    djvStringUtil.cpp
    djvSystem.cpp
    djvTime.cpp
    djvTimer.cpp
    djvUser.cpp
    djvVectorUtil.cpp
    djvWglContextPrivate.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_ADD_RESOURCES(rccSource djvCore.qrc)
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvCore_en_US.ts
        djvCore_cs_CZ.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_ADD_RESOURCES(rccSource djvCore.qrc)
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvCore_en_US.ts
        djvCore_cs_CZ.ts)
endif()

include_directories(${djvCoreIncludeDirs})

add_library(djvCore ${header} ${source} ${mocSource} ${rccSource} ${qmSource})

target_link_libraries(djvCore ${djvCoreDeps})

set_target_properties(djvCore PROPERTIES FOLDER lib)

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

