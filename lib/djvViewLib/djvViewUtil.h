//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvViewUtil.h

#ifndef DJV_VIEW_UTIL_H
#define DJV_VIEW_UTIL_H

#include <djvViewLibExport.h>

#include <djvError.h>
#include <djvStringUtil.h>
#include <djvVector.h>

#include <QMetaType>

class djvViewContext;

class djvFileInfo;
class djvPixelData;

#if defined(DJV_WINDOWS)

//! \todo Windows defines the macro "FILE_OPEN".

#undef FILE_OPEN

#endif

//! \addtogroup djvViewLib
//@{

//------------------------------------------------------------------------------
//! \class djvViewUtil
//!
//! This class provides enumerations and utilities.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewUtil
{
    Q_GADGET
    Q_ENUMS(VIEW_MAX)
    Q_ENUMS(TOOL_BAR)
    Q_ENUMS(GRID)
    Q_ENUMS(HUD)
    Q_ENUMS(HUD_BACKGROUND)
    Q_ENUMS(IMAGE_SCALE)
    Q_ENUMS(IMAGE_ROTATE)
    Q_ENUMS(PLAYBACK)
    Q_ENUMS(FRAME)
    Q_ENUMS(LOOP)
    Q_ENUMS(IN_OUT)
    Q_ENUMS(LAYOUT)
    Q_ENUMS(HISTOGRAM)
    Q_ENUMS(SHORTCUT)
    Q_ENUMS(MOUSE_WHEEL)
    Q_ENUMS(ZOOM_FACTOR)
    Q_ENUMS(ERROR)
    
public:

    //! Destructor.
    
    virtual ~djvViewUtil() = 0;

    //! This enumeration provides the maximum view size.

    enum VIEW_MAX
    {
        VIEW_MAX_NONE,
        VIEW_MAX_25,
        VIEW_MAX_50,
        VIEW_MAX_75,
        VIEW_MAX_USER,

        VIEW_MAX_COUNT
    };
    
    //! Get the maximum view size labels.
    
    static const QStringList & viewMaxLabels();

    //! This enumeration provides tool bar visibility options.

    enum TOOL_BAR
    {
        TOOL_BARS,
        PLAYBACK_BAR,
        INFO_BAR,

        TOOL_BAR_COUNT
    };

    //! Get the visibility option labels.

    static const QStringList & toolBarLabels();

    //! This enumeration provides the grid overlays.

    enum GRID
    {
        GRID_NONE,
        GRID_1x1,
        GRID_10x10,
        GRID_100x100,

        GRID_COUNT
    };

    //! Get the grid overlay labels.

    static const QStringList & gridLabels();

    //! This enumeration provides the HUD information.

    enum HUD
    {
        HUD_FILE_NAME,
        HUD_LAYER,
        HUD_SIZE,
        HUD_PROXY,
        HUD_PIXEL,
        HUD_TAG,
        HUD_FRAME,
        HUD_SPEED,

        HUD_COUNT
    };

    //! Get the HUD information labels.

    static const QStringList & hudInfoLabels();

    //! This enumeration provides the HUD backgrounds.

    enum HUD_BACKGROUND
    {
        HUD_BACKGROUND_NONE,
        HUD_BACKGROUND_SOLID,
        HUD_BACKGROUND_SHADOW,

        HUD_BACKGROUND_COUNT
    };

    //! Get the HUD background labels.

    static const QStringList & hudBackgroundLabels();
	
    //! This enumeration provides the image scales.

    enum IMAGE_SCALE
    {
        IMAGE_SCALE_NONE,
        IMAGE_SCALE_16_9,
        IMAGE_SCALE_1_0,
        IMAGE_SCALE_1_33,
        IMAGE_SCALE_1_78,
        IMAGE_SCALE_1_85,
        IMAGE_SCALE_2_0,
        IMAGE_SCALE_2_35,
        IMAGE_SCALE_2_39,
        IMAGE_SCALE_2_40,
        IMAGE_SCALE_1_1,
        IMAGE_SCALE_2_1,
        IMAGE_SCALE_3_2,
        IMAGE_SCALE_4_3,
        IMAGE_SCALE_5_3,
        IMAGE_SCALE_5_4,

        IMAGE_SCALE_COUNT
    };

    //! Get image scale labels.

    static const QStringList & imageScaleLabels();

    //! Calculate an image scale.

    static djvVector2f imageScale(IMAGE_SCALE, const djvVector2i &);

    //! This enumeration provides the image rotation.

    enum IMAGE_ROTATE
    {
        IMAGE_ROTATE_0,
        IMAGE_ROTATE_90,
        IMAGE_ROTATE_180,
        IMAGE_ROTATE_270,

        IMAGE_ROTATE_COUNT
    };

    //! Get image rotate labels.

    static const QStringList & imageRotateLabels();

    //! Get the image rotation in degrees.

    static double imageRotate(IMAGE_ROTATE);

    //! Load a LUT.

    static void loadLut(
        const djvFileInfo & fileInfo,
        djvPixelData &      lut,
        djvViewContext *    context) throw (djvError);

    //! This enumeration provides the playback modes.

    enum PLAYBACK
    {
        REVERSE,
        STOP,
        FORWARD,

        PLAYBACK_COUNT
    };

    //! Get the playback labels.

    static const QStringList & playbackLabels();

    //! This enumeration provides the frame controls.

    enum FRAME
    {
        FRAME_START,
        FRAME_START_ABS,
        FRAME_PREV,
        FRAME_PREV_10,
        FRAME_PREV_100,
        FRAME_NEXT,
        FRAME_NEXT_10,
        FRAME_NEXT_100,
        FRAME_END,
        FRAME_END_ABS,

        FRAME_COUNT
    };

    //! Get the frame labels.

    static const QStringList & frameLabels();

    //! This enumeration provides the loop modes.

    enum LOOP
    {
        LOOP_ONCE,
        LOOP_REPEAT,
        LOOP_PING_PONG,

        LOOP_COUNT
    };

    //! Get the loop mode labels.

    static const QStringList & loopLabels();

    //! This enumeration provides the in/out controls.

    enum IN_OUT
    {
        IN_OUT_ENABLE,
        MARK_IN,
        MARK_OUT,
        RESET_IN,
        RESET_OUT,

        IN_OUT_COUNT
    };

    //! Get the in/out labels.

    static const QStringList & inOutLabels();

    //! This enumeration provides the playback bar layouts.

    enum LAYOUT
    {
        LAYOUT_DEFAULT,
        LAYOUT_LEFT,
        LAYOUT_CENTER,
        LAYOUT_MINIMAL,

        LAYOUT_COUNT
    };

    //! Get the playback bar layout labels.

    static const QStringList & layoutLabels();
    
    //! This enumeration provides the tool actions.

    enum TOOL
    {
        TOOL_MAGNIFY,
        TOOL_COLOR_PICKER,
        TOOL_HISTOGRAM,
        TOOL_INFO,

        TOOL_COUNT
    };

    //! Get the tool action labels.

    static const QStringList & toolLabels();
    
    //! This enumeration provides the histogram sizes.
    
    enum HISTOGRAM
    {
        HISTOGRAM_256,
        HISTOGRAM_1024,
        HISTOGRAM_2048,
        HISTOGRAM_4096,
        
        HISTOGRAM_COUNT
    };

    //! Get the histogram size labels.

    static const QStringList & histogramLabels();
    
    //! Get a histogram size.
    
    static const int histogramSize(HISTOGRAM);

    //! This enumeration provides the keyboard shortcuts.

    enum SHORTCUT
    {
        SHORTCUT_EXIT,

        SHORTCUT_FILE_OPEN,
        SHORTCUT_FILE_RELOAD,
        SHORTCUT_FILE_RELOAD_FRAME,
        SHORTCUT_FILE_SAVE,
        SHORTCUT_FILE_SAVE_FRAME,
        SHORTCUT_FILE_CLOSE,
        SHORTCUT_FILE_LAYER_DEFAULT,
        SHORTCUT_FILE_LAYER_1,
        SHORTCUT_FILE_LAYER_2,
        SHORTCUT_FILE_LAYER_3,
        SHORTCUT_FILE_LAYER_4,
        SHORTCUT_FILE_LAYER_5,
        SHORTCUT_FILE_LAYER_6,
        SHORTCUT_FILE_LAYER_7,
        SHORTCUT_FILE_LAYER_8,
        SHORTCUT_FILE_LAYER_9,
        SHORTCUT_FILE_LAYER_10,
        SHORTCUT_FILE_LAYER_PREV,
        SHORTCUT_FILE_LAYER_NEXT,
        SHORTCUT_FILE_PROXY_NONE,
        SHORTCUT_FILE_PROXY_1_2,
        SHORTCUT_FILE_PROXY_1_4,
        SHORTCUT_FILE_PROXY_1_8,

        SHORTCUT_WINDOW_NEW,
        SHORTCUT_WINDOW_COPY,
        SHORTCUT_WINDOW_CLOSE,
        SHORTCUT_WINDOW_FIT,
        SHORTCUT_WINDOW_FULL_SCREEN,
        SHORTCUT_WINDOW_CONTROLS_VISIBLE,
        SHORTCUT_WINDOW_TOOL_BARS_VISIBLE,
        SHORTCUT_WINDOW_PLAYBACK_VISIBLE,
        SHORTCUT_WINDOW_INFO_VISIBLE,

        SHORTCUT_VIEW_LEFT,
        SHORTCUT_VIEW_RIGHT,
        SHORTCUT_VIEW_UP,
        SHORTCUT_VIEW_DOWN,
        SHORTCUT_VIEW_CENTER,
        SHORTCUT_VIEW_ZOOM_IN,
        SHORTCUT_VIEW_ZOOM_OUT,
        SHORTCUT_VIEW_ZOOM_RESET,
        SHORTCUT_VIEW_RESET,
        SHORTCUT_VIEW_FIT,
        SHORTCUT_VIEW_HUD,

        SHORTCUT_IMAGE_FRAME_STORE,
        SHORTCUT_IMAGE_FRAME_STORE_LOAD,
        SHORTCUT_IMAGE_MIRROR_HORIZONTAL,
        SHORTCUT_IMAGE_MIRROR_VERTICAL,
        SHORTCUT_IMAGE_SCALE_NONE,
        SHORTCUT_IMAGE_SCALE_16_9,
        SHORTCUT_IMAGE_SCALE_1_0,
        SHORTCUT_IMAGE_SCALE_1_33,
        SHORTCUT_IMAGE_SCALE_1_78,
        SHORTCUT_IMAGE_SCALE_1_85,
        SHORTCUT_IMAGE_SCALE_2_0,
        SHORTCUT_IMAGE_SCALE_2_35,
        SHORTCUT_IMAGE_SCALE_2_39,
        SHORTCUT_IMAGE_SCALE_2_40,
        SHORTCUT_IMAGE_SCALE_1_1,
        SHORTCUT_IMAGE_SCALE_2_1,
        SHORTCUT_IMAGE_SCALE_3_2,
        SHORTCUT_IMAGE_SCALE_4_3,
        SHORTCUT_IMAGE_SCALE_5_3,
        SHORTCUT_IMAGE_SCALE_5_4,
        SHORTCUT_IMAGE_ROTATE_0,
        SHORTCUT_IMAGE_ROTATE_90,
        SHORTCUT_IMAGE_ROTATE_180,
        SHORTCUT_IMAGE_ROTATE_270,
        SHORTCUT_IMAGE_COLOR_PROFILE,
        SHORTCUT_IMAGE_CHANNEL_RED,
        SHORTCUT_IMAGE_CHANNEL_GREEN,
        SHORTCUT_IMAGE_CHANNEL_BLUE,
        SHORTCUT_IMAGE_CHANNEL_ALPHA,
        SHORTCUT_IMAGE_DISPLAY_PROFILE,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_1,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_2,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_3,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_4,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_5,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_6,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_7,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_8,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_9,
        SHORTCUT_IMAGE_DISPLAY_PROFILE_10,

        SHORTCUT_PLAYBACK_REVERSE,
        SHORTCUT_PLAYBACK_STOP,
        SHORTCUT_PLAYBACK_FORWARD,
        SHORTCUT_PLAYBACK_TOGGLE,
        SHORTCUT_PLAYBACK_LOOP,
        SHORTCUT_PLAYBACK_START,
        SHORTCUT_PLAYBACK_START_ABS,
        SHORTCUT_PLAYBACK_PREV,
        SHORTCUT_PLAYBACK_PREV_10,
        SHORTCUT_PLAYBACK_PREV_100,
        SHORTCUT_PLAYBACK_NEXT,
        SHORTCUT_PLAYBACK_NEXT_10,
        SHORTCUT_PLAYBACK_NEXT_100,
        SHORTCUT_PLAYBACK_END,
        SHORTCUT_PLAYBACK_END_ABS,
        SHORTCUT_PLAYBACK_IN_OUT,
        SHORTCUT_PLAYBACK_MARK_IN,
        SHORTCUT_PLAYBACK_RESET_IN,
        SHORTCUT_PLAYBACK_MARK_OUT,
        SHORTCUT_PLAYBACK_RESET_OUT,

        SHORTCUT_TOOL_MAGNIFY,
        SHORTCUT_TOOL_COLOR_PICKER,
        SHORTCUT_TOOL_HISTOGRAM,
        SHORTCUT_TOOL_INFO,

        SHORTCUT_COUNT
    };

    //! Get the shortcut labels.

    static const QStringList & shortcutLabels();
    
    //! This enumeration provides the mouse wheel actions.

    enum MOUSE_WHEEL
    {
        MOUSE_WHEEL_VIEW_ZOOM,
        MOUSE_WHEEL_PLAYBACK_SHUTTLE,
        MOUSE_WHEEL_PLAYBACK_SPEED,

        MOUSE_WHEEL_COUNT
    };

    //! Get the mouse wheel action labels.

    static const QStringList & mouseWheelLabels();

    //! This enumeration provides the mouse wheel zoom factors.

    enum ZOOM_FACTOR
    {
        ZOOM_FACTOR_10,
        ZOOM_FACTOR_50,
        ZOOM_FACTOR_100,

        ZOOM_FACTOR_COUNT
    };

    //! Get the mouse wheel zoom factor labels.

    static const QStringList & zoomFactorLabels();
    
    //! Get the mouse wheel zoom factor.
    
    static double zoomFactor(ZOOM_FACTOR);

    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_OPEN_IMAGE,
        ERROR_READ_IMAGE,
        ERROR_WRITE_IMAGE,
        ERROR_OPEN_LUT,
        ERROR_PICK_COLOR,
        ERROR_HISTOGRAM,
        ERROR_MAGNIFY,

        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();
};

//------------------------------------------------------------------------------

DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::VIEW_MAX);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::TOOL_BAR);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::GRID);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::HUD_BACKGROUND);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::IMAGE_SCALE);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::IMAGE_ROTATE);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::PLAYBACK);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::FRAME);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::LOOP);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::LAYOUT);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::TOOL);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::HISTOGRAM);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::SHORTCUT);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::MOUSE_WHEEL);
DJV_STRING_OPERATOR(DJV_VIEW_LIB_EXPORT, djvViewUtil::ZOOM_FACTOR);

//@} // djvViewLib

#endif // DJV_VIEW_UTIL_H

