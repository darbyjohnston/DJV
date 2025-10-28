## 3.2.0

Features:
* Library updates:
    - OpenColorIO v2.4.2
    - OpenEXR v3.3.5
    - FFmpeg 8.0
    - USD v25.08
    - libpng v1.6.50,
    - libjpeg-turbo 3.1.1
    - SDL2 2.32.8
    - OpenSSL 3.5.2
    - curl 8_15_0
    - freetype 2-14-1
    - minizip 4.0.10

Fixes:
* Fixes for building with CMake 4.x versions.


## 3.1.1

Features:
* Make the timeline minimized by default.
* Add sample data for testing.

Fixes:
* Make the default color buffer RGBA_F32.
* More fixes for OpenColorIO settings.


## 3.1.0

Features:
* Add support for the OCIO environment variable and built-in OCIO configurations.
* Add more speed options.
* Add buttons to clear keyboard shortcuts.
* Add a setting for whether the file browser shows hidden files.
* Add position and zoom controls to the view tool, and a zoom control to the tool bar.
* Make the timeline ticks more visible.
* Add a keyboard shortcut (Ctrl+G) for enabling the grid.
* Add a keyboard shortcut (Ctrl+N) for toggling whether OCIO is enabled.
* Add a keyboard shortcut (Ctrl+K) for toggling whether the LUT is enabled.
* Add a Windows NSIS package.
* Use the current playback speed and layer when exporting.
* Add a setting for which mouse button is used for panning, compare wipe, color picker, and frame shuttle.
* Add a setting for scaling the mouse wheel zoom in the viewport and timeline.
* Add a button to manually save the settings.

Fixes:
* Fix for image sequences with uppercase file extensions.
* Fix for building with BMD support.
* Fix for OCIO settings not saving.
* Fix the timeline duration on the bottom tool bar to show the in/out range.
