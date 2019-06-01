set(html
    BuildLinux.html
    BuildWindows.html
    CommandLine.html
    Credits.html
    Documentation.html
    Download.html
    FAQ.html
    FileBrowser.html
    FileSequences.html
    ImageFormats.html
    Legal.html
    Screenshots.html
    Style.css
    Testimonials.html
	djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    index.html)
set(images
    "images/Berkeley 2010-12-19 Tinted.jpg"
    "images/Bodega Bay 2013-01-02 Tinted.jpg"
    "images/djvViewAnnotate.png"
    "images/djvViewAnnotateExport.png"
    "images/djvViewAnnotateSummary.png"
    "images/djvViewAnnotateUI.png"
    "images/djvViewColorPickerToolUI.png"
    "images/djvViewHistogramToolUI.png"
    "images/djvViewInformationToolUI.png"
    "images/djvViewMagnifyToolUI.png"
    "images/djvViewPlaybackUI.png"
    "images/djvViewUI.png"
    "images/FileBrowserUI.png"
    "images/GitHub_Logo.png"
    "images/Screenshot 1.png"
    "images/Screenshot 2.png"
    "images/index.png"
    "images/logo-filmreel.png"
	"images/unexpected_logo_white_150px.png")
set(legal
    ../LICENSE_CMake.txt
    ../LICENSE_FFmpeg.txt
    ../LICENSE_GLM.txt
    ../LICENSE_libjpeg.txt
    ../LICENSE_libpng.txt
    ../LICENSE_libtiff.txt
    ../LICENSE_OFL.txt
    ../LICENSE_OpenAL.txt
    ../LICENSE_OpenEXR.txt
    ../LICENSE_Qt.txt
    ../LICENSE.txt
    ../LICENSE_zlib.txt)

file(COPY ${html} DESTINATION ${DJV_BUILD_DIR}/doc)
file(COPY ${images} DESTINATION ${DJV_BUILD_DIR}/doc/images)

install(
    FILES
    ${html}
    DESTINATION ${DJV_INSTALL_DOC})
install(
    FILES
    ${images}
    DESTINATION ${DJV_INSTALL_DOC}/images)
#install(
#    DIRECTORY
#    doxygen/html
#    DESTINATION ${DJV_INSTALL_DOC}/doxygen)
install(
    FILES
    ${legal}
    DESTINATION ${DJV_INSTALL_DOC})

