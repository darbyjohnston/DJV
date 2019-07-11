set(INSTALL_NAME ${PROJECT_NAME}2)

if (WIN32)

    set(CPACK_GENERATOR ZIP NSIS)
    set(CMAKE_INSTALL_DEBUG_LIBRARIES true)	
	include(InstallRequiredSystemLibraries)
    	
    set(CPACK_NSIS_MUI_ICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv.ico)
    set(CPACK_NSIS_MUI_UNICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv.ico)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

    # \todo How can we ask the user if they want these file associations?    
	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	
		WriteRegStr HKCR '.cin' '' 'djv'

		WriteRegStr HKCR '.dpx' '' 'djv'

		WriteRegStr HKCR '.iff' '' 'djv'
		WriteRegStr HKCR '.z' '' 'djv'

		WriteRegStr HKCR '.ifl' '' 'djv'
		
		WriteRegStr HKCR '.jpeg' '' 'djv'
		WriteRegStr HKCR '.jpg' '' 'djv'
		WriteRegStr HKCR '.jfif' '' 'djv'

		WriteRegStr HKCR '.lut' '' 'djv'
		WriteRegStr HKCR '.1dl' '' 'djv'

		WriteRegStr HKCR '.exr' '' 'djv'

		WriteRegStr HKCR '.pic' '' 'djv'
		
		WriteRegStr HKCR '.png' '' 'djv'

		WriteRegStr HKCR '.ppm' '' 'djv'
		WriteRegStr HKCR '.pnm' '' 'djv'
		WriteRegStr HKCR '.pgm' '' 'djv'
		WriteRegStr HKCR '.pbm' '' 'djv'

		WriteRegStr HKCR '.avi' '' 'djv'
		WriteRegStr HKCR '.dv' '' 'djv'
		WriteRegStr HKCR '.gif' '' 'djv'
		WriteRegStr HKCR '.flv' '' 'djv'
		WriteRegStr HKCR '.mkv' '' 'djv'
		WriteRegStr HKCR '.mov' '' 'djv'
		WriteRegStr HKCR '.mpg' '' 'djv'
		WriteRegStr HKCR '.mpeg' '' 'djv'
		WriteRegStr HKCR '.mp4' '' 'djv'
		WriteRegStr HKCR '.m4v' '' 'djv'
		WriteRegStr HKCR '.mxf' '' 'djv'

		WriteRegStr HKCR '.rla' '' 'djv'
		WriteRegStr HKCR '.rpf' '' 'djv'

		WriteRegStr HKCR '.sgi' '' 'djv'
		WriteRegStr HKCR '.rgba' '' 'djv'
		WriteRegStr HKCR '.rgb' '' 'djv'
		WriteRegStr HKCR '.bw' '' 'djv'

		WriteRegStr HKCR '.tga' '' 'djv'
		
		WriteRegStr HKCR '.tiff' '' 'djv'
		WriteRegStr HKCR '.tif' '' 'djv'
		
		WriteRegStr HKCR 'djv' '' 'djv'
		WriteRegStr HKCR 'djv\\\\shell' '' 'open'
		WriteRegStr HKCR 'djv\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\bin\\\\djv.exe \\\"%1\\\"'
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

        DeleteRegKey HKCR 'djv'
	    ")
    
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${INSTALL_NAME})
	
elseif (APPLE)

    set(CPACK_GENERATOR Bundle)

    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/startup.sh.in
        ${PROJECT_BINARY_DIR}/startup.sh)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist)
        
    set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv.icns)
    set(CPACK_BUNDLE_NAME ${INSTALL_NAME})
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv.icns)
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_BINARY_DIR}/startup.sh)

elseif (UNIX)
        
    set(CPACK_RPM_PACKAGE_NAME ${INSTALL_NAME})
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
    set(CPACK_RPM_PACKAGE_RELOCATABLE true)
    if(DJV_THIRD_PARTY)
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
        
    set(CPACK_DEBIAN_PACKAGE_NAME ${INSTALL_NAME})
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgcc1, libgl1")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
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

set(CPACK_PACKAGE_EXECUTABLES djv djv)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)

