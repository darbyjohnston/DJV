set(html
    Build.html
    BuildLinux.html
    BuildWindows.html
    CommandLine.html
    Credits.html
    Documentation.html
    Download.html
    FAQ.html
    FileBrowser.html
    ImageFormats.html
    Legal.html
    Screenshots.html
    Style.css
    Testimonials.html
    ViewCommandLine.html
    ViewFiles.html
    ViewImageView.html
    ViewImages.html
    ViewPlayback.html
    ViewTools.html
    ViewWindows.html
    djv_convert.html
    djv_info.html
    djv_ls.html
    djv_view.html
    index.html)
set(images
    "images/Berkeley 2010-12-19 Tinted.jpg"
    "images/DownloadIcon.png"
    "images/FileCloseIcon96DPI.png"
    "images/FileIcon96DPI.png"
    "images/FileOpenIcon96DPI.png"
    "images/FileReloadIcon96DPI.png"
    "images/Miscellaneous Preferences.png"
    "images/Screenshot 1.png"
    "images/Screenshot 2.png"
    "images/Screenshot 3.png"
    "images/WindowCloseIcon96DPI.png"
    "images/WindowDuplicateIcon96DPI.png"
    "images/WindowFitIcon96DPI.png"
    "images/WindowFullScreenIcon96DPI.png"
    "images/WindowNewIcon96DPI.png"
    "images/index.png"
    "images/logo-filmreel.png")
set(legal
    ../LICENSE_CMake.txt
    ../LICENSE_FFmpeg.txt
    ../LICENSE_GLM.txt
    ../LICENSE_libjpeg.txt
    ../LICENSE_libpng.txt
    ../LICENSE_libtiff.txt
    ../LICENSE_OpenAL.txt
    ../LICENSE_OpenEXR.txt
    ../LICENSE_Qt.txt
    ../LICENSE.txt
    ../LICENSE_zlib.txt)

file(COPY ${html} DESTINATION ${djvBuildDir}/doc)
file(COPY ${images} DESTINATION ${djvBuildDir}/doc/images)

install(
    FILES
    ${html}
    DESTINATION doc)
install(
    FILES
    ${images}
    DESTINATION doc/images)
#install(
#    DIRECTORY
#    doxygen/html
#    DESTINATION doc/doxygen)
install(
    FILES
    ${legal}
    DESTINATION doc)
