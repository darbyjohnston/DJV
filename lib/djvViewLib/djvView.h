//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvView.h

#ifndef DJV_VIEW_H
#define DJV_VIEW_H

#include <djvViewLibExport.h>

#include <djvVector.h>

#include <QString>

class djvFileInfo;
class djvPixelData;

//! \addtogroup djvViewLib
//@{

//------------------------------------------------------------------------------
//! \struct djvView
//!
//! This struct provides basic functionality.
//------------------------------------------------------------------------------

struct DJV_VIEW_LIB_EXPORT djvView
{
    //! This enumeration provides the maximum percentage of the screen to use
    //! when resizing a window to fit an image.

    enum WINDOW_RESIZE_MAX
    {
        WINDOW_RESIZE_MAX_UNLIMITED,
        WINDOW_RESIZE_MAX_25,
        WINDOW_RESIZE_MAX_50,
        WINDOW_RESIZE_MAX_75,

        WINDOW_RESIZE_MAX_COUNT
    };
    
    //! Get the window maximum resize labels.
    
    static const QStringList & windowResizeMaxLabels();

    //! Get the maximum percentage of the screen to use when resizing a window
    //! to fit an image.

    static double windowResizeMax(WINDOW_RESIZE_MAX);

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
	
    //! This enumeration provides the view resize behavior.

    enum VIEW_RESIZE
    {
        VIEW_RESIZE_NONE,
        VIEW_RESIZE_FIT_IMAGE,
        VIEW_RESIZE_CENTER_IMAGE,

        VIEW_RESIZE_COUNT
    };

    //! Get the view resize behavior labels.

    static const QStringList & viewResizeLabels();

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

    static void loadLut(const djvFileInfo &, djvPixelData & lut);

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
};

//------------------------------------------------------------------------------

DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::WINDOW_RESIZE_MAX &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::TOOL_BAR &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::VIEW_RESIZE &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::GRID &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::HUD_BACKGROUND &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::IMAGE_SCALE &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::IMAGE_ROTATE &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::PLAYBACK &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::FRAME &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::LOOP &) throw (QString);
DJV_VIEW_LIB_EXPORT QStringList & operator >> (QStringList &,
    djvView::LAYOUT &) throw (QString);

DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::WINDOW_RESIZE_MAX);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::TOOL_BAR);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::VIEW_RESIZE);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::GRID);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::HUD_BACKGROUND);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::IMAGE_SCALE);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::IMAGE_ROTATE);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::PLAYBACK);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::FRAME);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::LOOP);
DJV_VIEW_LIB_EXPORT QStringList & operator << (QStringList &,
    djvView::LAYOUT);

//@} // djvViewLib

#endif // DJV_VIEW_H

