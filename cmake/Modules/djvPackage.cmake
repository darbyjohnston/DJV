set(djvPackageName ${djvProjectName}-${djvVersion}-${djvSystemName}-${djvBuildArch})

if (WIN32)

    set(CPACK_GENERATOR ZIP NSIS)
    set(CMAKE_INSTALL_DEBUG_LIBRARIES true)	
	include(InstallRequiredSystemLibraries)
    	
    set(CPACK_NSIS_MUI_ICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)
    set(CPACK_NSIS_MUI_UNICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

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

		WriteRegStr HKCR '.avi' '' 'djv_view'
		WriteRegStr HKCR '.dv' '' 'djv_view'
		WriteRegStr HKCR '.gif' '' 'djv_view'
		WriteRegStr HKCR '.flv' '' 'djv_view'
		WriteRegStr HKCR '.mkv' '' 'djv_view'
		WriteRegStr HKCR '.mov' '' 'djv_view'
		WriteRegStr HKCR '.mpg' '' 'djv_view'
		WriteRegStr HKCR '.mpeg' '' 'djv_view'
		WriteRegStr HKCR '.mp4' '' 'djv_view'
		WriteRegStr HKCR '.m4v' '' 'djv_view'
		WriteRegStr HKCR '.mxf' '' 'djv_view'

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

		DeleteRegKey HKCR '.avi'
		DeleteRegKey HKCR '.dv'
		DeleteRegKey HKCR '.gif'
		DeleteRegKey HKCR '.flv'
		DeleteRegKey HKCR '.mkv'
		DeleteRegKey HKCR '.mov'
		DeleteRegKey HKCR '.mpg'
		DeleteRegKey HKCR '.mpeg'
		DeleteRegKey HKCR '.mp4'
		DeleteRegKey HKCR '.m4v'

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
    
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${djvPackageName})
	
elseif (APPLE)

    set(CPACK_GENERATOR Bundle)

    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/startup.sh.in
        ${PROJECT_BINARY_DIR}/startup.sh)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist)
        
    set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_NAME ${djvPackageName})
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_BINARY_DIR}/startup.sh)

elseif (UNIX)

    #set(CPACK_GENERATOR TGZ RPM DEB)
    set(CPACK_GENERATOR RPM)
        
    set(CPACK_RPM_PACKAGE_NAME djv)
    set(CPACK_RPM_PACKAGE_RELOCATABLE true)
    if(djvThirdPartyPackage)
        set(CPACK_RPM_PACKAGE_AUTOREQPROV false)
    endif()
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST
        /usr
        /usr/bin
        /usr/include
        /usr/lib
        /usr/share
        /usr/share/applications
        /usr/share/icons
        /usr/share/mime
        /usr/share/mime/packages
        /usr/share/icons/hicolor
        /usr/share/icons/hicolor/scalable
        /usr/share/icons/hicolor/scalable/apps)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/install.sh.in
        ${PROJECT_BINARY_DIR}/etc/Linux/install.sh
        @ONLY)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/uninstall.sh.in
        ${PROJECT_BINARY_DIR}/etc/Linux/uninstall.sh
        @ONLY)
    SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE
        ${PROJECT_BINARY_DIR}/etc/Linux/install.sh)
    SET(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE
        ${PROJECT_BINARY_DIR}/etc/Linux/uninstall.sh)
        
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Media review software for VFX artists and filmmakers")
set(CPACK_PACKAGE_EXECUTABLES djv_view djv_view)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_PACKAGE_FILE_NAME ${djvPackageName})

