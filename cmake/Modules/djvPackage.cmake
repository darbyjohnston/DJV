set(djvPackageName ${djvProjectName}-${djvSystemName}-${djvBuildArch})

#message("djvPackageName = " ${djvPackageName})

if (WIN32)

    # Set the type of package to build.

    set(CPACK_GENERATOR ZIP NSIS)
    
    # Add required system libraries to the package.
    
	set(CMAKE_INSTALL_DEBUG_LIBRARIES true)
	
	include(InstallRequiredSystemLibraries)

    # Set NSIS package options.
    	
    set(CPACK_NSIS_MUI_ICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)
    set(CPACK_NSIS_MUI_UNICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)

    if (djvBuildArch EQUAL 32)
    
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    
    elseif (djvBuildArch EQUAL 64)

        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

    endif (djvBuildArch EQUAL 32)
	
    # \todo How can we ask the user if they want these file associations?
    
	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	
		WriteRegStr HKCR '.cin' '' 'djv_view'

		WriteRegStr HKCR '.dpx' '' 'djv_view'

		WriteRegStr HKCR '.iff' '' 'djv_view'
		WriteRegStr HKCR '.z' '' 'djv_view'

		WriteRegStr HKCR '.ifl' '' 'djv_view'
		
		WriteRegStr HKCR '.jpeg' '' 'djv_view'
		WriteRegStr HKCR '.jpg' '' 'djv_view'
		WriteRegStr HKCR '.jfif' '' 'djv_view'

		WriteRegStr HKCR '.lut' '' 'djv_view'
		WriteRegStr HKCR '.1dl' '' 'djv_view'

		WriteRegStr HKCR '.exr' '' 'djv_view'

		WriteRegStr HKCR '.pic' '' 'djv_view'
		
		WriteRegStr HKCR '.png' '' 'djv_view'

		WriteRegStr HKCR '.ppm' '' 'djv_view'
		WriteRegStr HKCR '.pnm' '' 'djv_view'
		WriteRegStr HKCR '.pgm' '' 'djv_view'
		WriteRegStr HKCR '.pbm' '' 'djv_view'

		WriteRegStr HKCR '.qt' '' 'djv_view'
		WriteRegStr HKCR '.mov' '' 'djv_view'
		WriteRegStr HKCR '.mp4' '' 'djv_view'

		WriteRegStr HKCR '.rla' '' 'djv_view'
		WriteRegStr HKCR '.rpf' '' 'djv_view'

		WriteRegStr HKCR '.sgi' '' 'djv_view'
		WriteRegStr HKCR '.rgba' '' 'djv_view'
		WriteRegStr HKCR '.rgb' '' 'djv_view'
		WriteRegStr HKCR '.bw' '' 'djv_view'

		WriteRegStr HKCR '.tga' '' 'djv_view'
		
		WriteRegStr HKCR '.tiff' '' 'djv_view'
		WriteRegStr HKCR '.tif' '' 'djv_view'
		
		WriteRegStr HKCR 'djv_view' '' 'djv_view'
		WriteRegStr HKCR 'djv_view\\\\shell' '' 'open'
		WriteRegStr HKCR 'djv_view\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\bin\\\\djv_view.exe \\\"%1\\\"'
		")

	set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
 
		DeleteRegKey HKCR '.cin'

		DeleteRegKey HKCR '.dpx'

		DeleteRegKey HKCR '.iff'
		DeleteRegKey HKCR '.z'

		DeleteRegKey HKCR '.ifl'

		DeleteRegKey HKCR '.jpeg'
		DeleteRegKey HKCR '.jpg'
		DeleteRegKey HKCR '.jfif'
		
		DeleteRegKey HKCR '.lut'
		DeleteRegKey HKCR '.1dl'

		DeleteRegKey HKCR '.exr'

		DeleteRegKey HKCR '.pic'

		DeleteRegKey HKCR '.png'

		DeleteRegKey HKCR '.ppm'
		DeleteRegKey HKCR '.pnm'
		DeleteRegKey HKCR '.pgm'
		DeleteRegKey HKCR '.pbm'

		DeleteRegKey HKCR '.qt'
		DeleteRegKey HKCR '.mov'
		DeleteRegKey HKCR '.mp4'

		DeleteRegKey HKCR '.rla'
		DeleteRegKey HKCR '.rpf'

		DeleteRegKey HKCR '.sgi'
		DeleteRegKey HKCR '.rgba'
		DeleteRegKey HKCR '.rgb'
		DeleteRegKey HKCR '.bw'

		DeleteRegKey HKCR '.tga'

		DeleteRegKey HKCR '.tiff'
		DeleteRegKey HKCR '.tif'

        DeleteRegKey HKCR 'djv_view'
	    ")

    # Set the installation directory.
    
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${djvPackageName})
	
elseif (APPLE)

    # Set the type of package to build.
    
    set(CPACK_GENERATOR Bundle)

    # Add bundle files to the package.
    
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/startup.sh.in
        ${PROJECT_BINARY_DIR}/startup.sh)

    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist)
    
    # Set bundle options.
    
    set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_NAME ${djvPackageName})
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_BINARY_DIR}/startup.sh)

elseif (UNIX)

    # Set the type of package to build.

    set(CPACK_GENERATOR TGZ RPM DEB)

    # Set the installation directory.
    
    set(CPACK_SET_DESTDIR true)
    set(CPACK_INSTALL_PREFIX /usr/local/${djvPackageName})
    
    # Set RPM options.
    
    set(CPACK_RPM_PACKAGE_NAME djv)
    #set(CPACK_RPM_PACKAGE_RELOCATABLE true)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/install.sh.in
        ${PROJECT_BINARY_DIR}/etc/Linux/install.sh
        @ONLY)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/uninstall.sh.in
        ${PROJECT_BINARY_DIR}/etc/Linux/uninstall.sh
        @ONLY)
    SET(
        CPACK_RPM_POST_INSTALL_SCRIPT_FILE
        ${PROJECT_BINARY_DIR}/etc/Linux/install.sh)
    SET(
        CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE
        ${PROJECT_BINARY_DIR}/etc/Linux/uninstall.sh)
    
    # Set Debian options.
    
    set(CPACK_DEBIAN_PACKAGE_NAME djv)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/postinst.in
        ${PROJECT_BINARY_DIR}/etc/Linux/postinst
        @ONLY)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/postrm.in
        ${PROJECT_BINARY_DIR}/etc/Linux/postrm
        @ONLY)
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
        ${PROJECT_BINARY_DIR}/etc/Linux/postinst
        ${PROJECT_BINARY_DIR}/etc/Linux/postrm)

endif (WIN32)

set(CPACK_PACKAGE_VERSION_MAJOR ${djvVersionMajor})
set(CPACK_PACKAGE_VERSION_MINOR ${djvVersionMinor})
set(CPACK_PACKAGE_VERSION_PATCH ${djvVersionPatch})
set(CPACK_PACKAGE_CONTACT http://djv.sourceforge.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/README)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "DJV Imaging provides professional movie playback software for use in film, VFX, and computer animation.")

set(CPACK_PACKAGE_EXECUTABLES djv_view djv_view)

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)

set(CPACK_PACKAGE_FILE_NAME ${djvPackageName})

