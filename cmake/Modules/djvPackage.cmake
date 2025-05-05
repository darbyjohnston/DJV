 set(CPACK_PACKAGE_FILE_NAME
    ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

if(WIN32)
    set(CPACK_GENERATOR ZIP)
    
    set(INSTALL_DLLS)
    
    if(TLRENDER_FFMPEG)
        set(FFMPEG_DLLS
            ${CMAKE_INSTALL_PREFIX}/bin/avcodec-61.dll
            ${CMAKE_INSTALL_PREFIX}/bin/avdevice-61.dll
            ${CMAKE_INSTALL_PREFIX}/bin/avformat-61.dll
            ${CMAKE_INSTALL_PREFIX}/bin/avutil-59.dll
            ${CMAKE_INSTALL_PREFIX}/bin/swresample-5.dll
            ${CMAKE_INSTALL_PREFIX}/bin/swscale-8.dll)
        list(APPEND INSTALL_DLLS ${FFMPEG_DLLS})
    endif()
    
    if(TLRENDER_USD)
        set(MATERIALX_DLLS
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXCore.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXFormat.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXGenGlsl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXGenMdl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXGenMsl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXGenOsl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXGenShader.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXRender.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXRenderGlsl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXRenderHw.dll
            ${CMAKE_INSTALL_PREFIX}/bin/MaterialXRenderOsl.dll)
        set(TBB_DLLS
            ${CMAKE_INSTALL_PREFIX}/bin/tbb.dll)
        set(USD_DLLS
            ${CMAKE_INSTALL_PREFIX}/bin/usd_ar.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_arch.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_cameraUtil.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_garch.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_geomUtil.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_gf.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_glf.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hd.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdGp.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdMtlx.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdSt.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdar.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdsi.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hdx.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hf.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hgi.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hgiGL.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hgiInterop.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_hio.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_js.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_kind.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_ndr.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_pcp.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_pegtl.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_plug.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_pxOsd.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_sdf.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_sdr.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_tf.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_trace.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_ts.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usd.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdAppUtils.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdBakeMtlx.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdGeom.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdHydra.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdImaging.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdImagingGL.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdLux.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdMedia.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdMtlx.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdPhysics.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdProc.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdProcImaging.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdRender.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdRi.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdRiPxrImaging.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdShade.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdSkel.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdSkelImaging.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdUI.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdUtils.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdVol.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_usdVolImaging.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_vt.dll
            ${CMAKE_INSTALL_PREFIX}/bin/usd_work.dll)
        list(APPEND INSTALL_DLLS ${MATERIALX_DLLS} ${TBB_DLLS} ${USD_DLLS})

        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/usd
            DESTINATION bin)
        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/plugin
            DESTINATION ".")
    endif()
    
    install(
        FILES ${INSTALL_DLLS}
        DESTINATION bin)

elseif(APPLE)

    set(CPACK_GENERATOR Bundle)

    list(APPEND CMAKE_INSTALL_RPATH
        "@executable_path/../lib")
    #set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(INSTALL_DYLIBS)
    
    if(TLRENDER_FFMPEG)
        set(FFMPEG_DYLIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.61.19.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.61.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.61.3.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.61.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.61.7.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.61.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.59.39.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.59.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.5.3.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.5.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.8.3.100.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.8.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.dylib)
        list(APPEND INSTALL_DYLIBS ${FFMPEG_DYLIBS})
    endif()
    
    if(TLRENDER_USD)
        set(MATERIALX_DYLIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderMsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderMsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderMsl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.1.39.3.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.1.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.dylib)
        set(TBB_DYLIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb_debug.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_debug.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy_debug.dylib)
        set(OSD_DYLIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libosdCPU.3.6.0.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libosdCPU.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libosdGPU.3.6.0.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libosdGPU.dylib)
        set(USD_DYLIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ar.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_arch.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_cameraUtil.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_garch.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_geomUtil.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_gf.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_glf.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hd.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdGp.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdMtlx.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdSt.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdar.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdsi.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdx.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hf.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgi.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgiGL.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgiInterop.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgiMetal.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hio.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_js.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_kind.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ndr.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pcp.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pegtl.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_plug.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pxOsd.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_sdf.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_sdr.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_tf.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_trace.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ts.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usd.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdAppUtils.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdBakeMtlx.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdGeom.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdHydra.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdImaging.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdImagingGL.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdLux.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdMedia.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdMtlx.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdPhysics.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdProc.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdProcImaging.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRender.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRi.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRiPxrImaging.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdShade.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdSkel.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdSkelImaging.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdUI.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdUtils.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdVol.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdVolImaging.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_vt.dylib
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_work.dylib)
        list(APPEND INSTALL_DYLIBS ${MATERIALX_DYLIBS} ${TBB_DYLIBS} ${OSD_DYLIBS} ${USD_DYLIBS})

        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/usd
            DESTINATION lib)
        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/plugin
            DESTINATION ".")
    endif()
    
    install(
        FILES ${INSTALL_DYLIBS}
        DESTINATION lib)

    set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/macOS/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/etc/macOS/DJV.icns)

else()

    set(CPACK_GENERATOR TGZ)

    set(INSTALL_LIBS)
    
    if(TLRENDER_FFMPEG)
        set(FFMPEG_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so.61
            ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so.61.19.100
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so.61
            ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so.61.3.100
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so.61
            ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so.61.7.100
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so.59
            ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so.59.39.100
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so.5
            ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so.5.3.100
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so.8
            ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so.8.3.100)
        list(APPEND INSTALL_LIBS ${FFMPEG_LIBS})
    endif()

    if(TLRENDER_USD)
        set(MATERIALX_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXCore.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXFormat.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenGlsl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMdl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenMsl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenOsl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXGenShader.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderGlsl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderHw.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRenderOsl.so.1.39.3
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.so
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.so.1
            ${CMAKE_INSTALL_PREFIX}/lib/libMaterialXRender.so.1.39.3)
        set(TBB_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb_debug.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb_debug.so.2
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_debug.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_debug.so.2
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy_debug.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy_debug.so.2
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc_proxy.so.2
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbbmalloc.so.2
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb.so
            ${CMAKE_INSTALL_PREFIX}/lib/libtbb.so.2)
        set(OSD_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libosdCPU.so
            ${CMAKE_INSTALL_PREFIX}/lib/libosdCPU.so.3.6.0
            ${CMAKE_INSTALL_PREFIX}/lib/libosdGPU.so
            ${CMAKE_INSTALL_PREFIX}/lib/libosdGPU.so.3.6.0)
        set(USD_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_arch.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ar.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_cameraUtil.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_garch.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_geomUtil.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_gf.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_glf.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdar.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdGp.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdMtlx.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdsi.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hd.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdSt.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hdx.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hf.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgiGL.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgiInterop.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hgi.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_hio.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_js.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_kind.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ndr.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pcp.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pegtl.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_plug.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_pxOsd.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_sdf.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_sdr.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_tf.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_trace.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_ts.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdAppUtils.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdBakeMtlx.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdGeom.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdHydra.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdImagingGL.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdImaging.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdLux.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdMedia.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdMtlx.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdPhysics.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdProcImaging.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdProc.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRender.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRi.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdRiPxrImaging.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdShade.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdSkelImaging.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdSkel.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usd.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdUI.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdUtils.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdVolImaging.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_usdVol.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_vt.so
            ${CMAKE_INSTALL_PREFIX}/lib/libusd_work.so)
        list(APPEND INSTALL_LIBS ${MATERIALX_LIBS} ${TBB_LIBS} ${OSD_LIBS} ${USD_LIBS})

        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/usd
            DESTINATION lib)
        install(
            DIRECTORY ${CMAKE_INSTALL_PREFIX}/plugin
            DESTINATION ".")
    endif()
    
    install(
        FILES ${INSTALL_LIBS}
        DESTINATION lib)

endif()

