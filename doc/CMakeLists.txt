set(html
    Annotations.html
    BuildLinux.html
    BuildWindows.html
	ChangeList.html
	ColorSpaces.html
    Credits.html
    Documentation.html
	Donate.html
    Download.html
	Files.html
	ImageView.html
    Legal.html
	Playback.html
    Screenshots.html
    Style.css
    Testimonials.html
	UI.html
    index.html)
set(images
	"images/ColorSpaceSettings1.png"
	"images/ColorSpaceSettings2.png"
	"images/ColorSpaceWidget.png"
	"images/FilesMaximized.png"
	"images/FilesMemoryCache.png"
	"images/FilesOpeningAndClosing.png"
	"images/FilesRecent.png"
	"images/FilesUnmaximized.png"
	"images/GitHub_Logo_White.png"
	"images/ImageViewLocking.png"
	"images/ImageViewPanningAndZooming.png"
	"images/Screenshot1.png"
	"images/Screenshot2.png"
	"images/Screenshot3.png"
	"images/UIAutoHide.png"
    "images/djv-logo-large.png"
    "images/djv-logo-small.png"
    "images/index.png"
	"images/unexpected_logo_white_150px.png")
set(legal
    ../LICENSE.txt
    ../LICENSE_CMake.txt
    ../LICENSE_FFmpeg.txt
    ../LICENSE_FreeType.txt
    ../LICENSE_GLFW.md
    ../LICENSE_GLM.txt
    ../LICENSE_Icons.txt
    ../LICENSE_MbedTLS.txt
    ../LICENSE_OFL.txt
    ../LICENSE_OpenAL.txt
    ../LICENSE_OCIO.txt
    ../LICENSE_OpenEXR.txt
    ../LICENSE_curl.txt
    ../LICENSE_dr_libs.txt
    ../LICENSE_glad.txt
    ../LICENSE_libjpeg.txt
    ../LICENSE_libjpeg-turbo.txt
    ../LICENSE_libpng.txt
    ../LICENSE_libtiff.txt
    ../LICENSE_zlib.txt
    ../README.md)

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

