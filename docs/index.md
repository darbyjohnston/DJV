# DJV ![Main Window](assets/DJV_Icon_32.svg)

DJV is an application for playback and review of image sequences.

Features include:
* Compare multiple files with wipe, overlay, and difference modes.
* Timeline support with OpenTimelineIO.
* Color management with OpenColorIO.
* Multi-track audio with variable speed and reverse playback.
* Experimental support for USD files.
* Available for Linux, macOS, and Windows.

![Main Window](assets/djv_screenshot1.png)


<br><br>
### Documentation

1. [Download and Install](#install)
2. [Main Window](#main_window)
3. [Working with Files](#files)
4. [Playback and Frame Control](#playback)
5. [A/B Comparison](#compare)
6. [Color Management](#color)
7. [Exporting Files](#export)
8. [Keyboard Shortcuts](#shortcuts)
9. [Building from Source](#build)


<br><br><a name="install"></a>
## Download and Install

**NOTE**: Download packages only include a minimal set of video and audio
codecs. To enable full support, either compile from source or replace the
FFmpeg shared libraries.

### Linux and Windows

Packages are distributed as ZIP archives. Unzip the archive and move the DJV
folder to a convenient location.

### macOS

Packages are distributed as macOS disk images. Open the disk image and copy
DJV to the Applications folder.


<br><br><a name="main_window"></a>
## Main Window

![Main Window](assets/MainWindowAnnotated.svg)

Main window components:

1. Menu Bar - Menus and actions for controlling the application.
2. Tool Bars - Access to commonly used actions.
3. Tab Bar - Switch between currently opened files.
4. Viewport - The view of the current file.
5. Timeline - The timeline for the current file.
6. Bottom Bar - Playback, frame, and audio controls.
7. Tool Widget - The current tool widget.
8. Status Bar - Errors and information about the current file.

Components can be toggled on and off from the **Window** menu.

Splitters can be used to change the size of the components.

Full screen mode can be enabled from the **Window** menu.

A secondary window can be shown to mirror the viewport on a separate monitor.
The secondary window can be shown from the **Window** menu.

### Viewport

Viewport controls:
* Zoom - Mouse wheel or keyboard shortcuts '-' and '='
* Frame view - Keyboard shortcut 'Backspace'
* Pan  - 'Ctrl' + mouse click and drag
* Compare wipe - 'Alt' + mouse click and drag
* Color picker - Mouse click
* Frame shuttle: Mouse click and drag

Viewport controls can be customized in the **Settings** tool.

Information can be overlaid on the viewport by enabling the HUD (heads up
display). The HUD can be enabled from the **View** menu.

![Viewport HUD](assets/ViewportHUDAnnotated.svg)

1. Current file name
2. Current frame
3. Playback speed
4. Number of frames dropped during playback
5. Color picker
6. Video cache usage
7. Audio cache usage


<br><br><a name="files"></a>
## Working with Files

Supported formats:
* Image sequences: Cineon, DPX, JPEG, OpenEXR, PNG, PPM, SGI, TGA, BMP, TIFF
* Movie codecs: MJPEG
* Audio codecs: FLAC, MP3, WAV
* Timelines: OTIO, OTIOZ

Files and folders can be opened from the **File** menu, by dragging and
dropping onto the main window, or from the command line. Opening a folder will
open all of the media files in the folder (non-recursively).

The native file browser is enabled by default on Windows and macOS. The
built-in file browser can be enabled in the **Settings** tool.

The current file can be changed from the **File/Current** menu, the
**Tab Bar**, or the **Files** tool.

### Memory Cache

The memory cache can be configured in the **Settings** tool. There are
separate values for video, audio, and "read behind". The "read behind"
value is the number of seconds that are read before the current frame. This
is useful to ensure frames are cached when scrubbing.

Only the current file is stored in the cache. When the current file is
changed, it is unloaded from the cache and the new file is loaded.

### Layers

For files that contain multiple layers (i.e., OpenEXR), the current layer can
be changed from the **File/Layers** menu or the **Files** tool.

### Image Sequences and Audio

Audio can be added to image sequences either automatically or explicitly.

To automatically add audio to image sequences, open the **Image Sequences**
section in the **Settings Tool**. Audio files can be found by either specifying
the file extensions to search for (e.g., ".wav .mp3"), or providing a specific
file name.

To explicitly add audio to an image sequence use the
**File/Open With Separate Audio** menu.


<br><br><a name="playback"></a>
## Playback and Frame Control

![Playback Controls](assets/PlaybackControlsAnnotated.svg)

1. Playback controls
2. Playback shuttle - Click and drag to change playback speed
3. Frame controls
4. Frame shuttle - Click and drag to change the current frame
5. Current frame
6. Duration
7. Current speed
8. Time units (frames, seconds, timecode)

In and out points can be set frome the **Playback** menu to limit playback to a
section of the timeline.

The number of dropped frames during playback can be viewed in the HUD, which is
available from the **View** menu.

### Timeline

![Timeline](assets/TimelineAnnotated.svg)

1. Current frame
2. Video and audio cache display
3. Video track
4. Video clips
5. Audio track
6. Audio clips

Controls:
* Current frame: Mouse click and drag
* Zoom - Mouse wheel or keyboard shortcuts '-' and '='
* Frame view - Keyboard shortcut 'Backspace'
* Pan - 'Ctrl' + mouse click and drag

The size of the timeline thumbnails can be set from the **Timeline** menu.
Thumbnails can also be disabled to improve performance.


<br><br><a name="compare"></a>
## A/B Comparison

Two or more files can be compared:
* A - Show only the "A" file
* B - Show only the "B" file
* Wipe - Wipe between the "A" and "B" files
* Overlay - Show the "B" file over the "A" file
* Difference - Show the difference between the "A" and "B" files
* Horizontal - Show the "A" and "B" files side by side
* Vertical - Show the "A" and "B" files over and under
* Tile - Show the "A" and multiple "B" files as tiles

To compare files, open both files and set the "B" file from either the
**Compare/B** menu or **Files** tool. The "A" file is the current file.

Files can be compared in relative or absolute time mode. In relative time mode
the time of the "B" file will be offset to match the start of the "A" file. In
absolute time mode the "A" and "B" times will be the same.


<br><br><a name="color"></a>
## Color Management


<br><br><a name="export"></a>
## Exporting Files


<br><br><a name="shortcuts"></a>
## Keyboard Shortcuts


<br><br><a name="build"></a>
## Building from Source

### Building on Linux

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV/etc/SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=$PWD/Release/install -DCMAKE_PREFIX_PATH=$PWD/Release/install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
./Release/DJV/src/DJV-build/bin/djv/djv
```

### Building on macOS

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV/etc/SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=$PWD/Release/install -DCMAKE_PREFIX_PATH=$PWD/Release/install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
./Release/DJV/src/DJV-build/bin/djv/djv
```

The CMake variable `CMAKE_OSX_ARCHITECTURES` can be used to specify the build
architecture:
```
-DCMAKE_OSX_ARCHITECTURES=x86_64
```
```
-DCMAKE_OSX_ARCHITECTURES=arm64
```

These aliases are convenient for switching between architectures:
```
alias arm="env /usr/bin/arch -arm64 /bin/zsh --login"
alias intel="env /usr/bin/arch -x86_64 /bin/zsh --login"
```

### Building on Windows

Dependencies:
* Install MSYS2 (https://www.msys2.org) for compiling FFmpeg.
* Install Strawberry Perl (https://strawberryperl.com/) for compiling network support.
* Install Python 3.11 for compiling USD.

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV\etc\SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=%CD%\Release\install -DCMAKE_PREFIX_PATH=%CD%\Release\install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
set PATH=%CD%\Release\install\bin;%PATH%
```
```
.\Release\DJV\src\DJV-build\bin\djv\Release\djv
```
