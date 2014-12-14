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

//! \file djvView.cpp

#include <djvView.h>

#include <djvApplication.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvVectorUtil.h>

//------------------------------------------------------------------------------
// djvView
//------------------------------------------------------------------------------

const QStringList & djvView::windowResizeMaxLabels()
{
    static const QStringList data = QStringList() <<
        "Unlimited" <<
        "25%" <<
        "50%" <<
        "75%";

    DJV_ASSERT(data.count() == WINDOW_RESIZE_MAX_COUNT);

    return data;
}

double djvView::windowResizeMax(WINDOW_RESIZE_MAX in)
{
    switch (in)
    {
        case WINDOW_RESIZE_MAX_25: return 0.25;
        case WINDOW_RESIZE_MAX_50: return 0.50;
        case WINDOW_RESIZE_MAX_75: return 0.75;

        default: break;
    }

    return 1.0;
}

const QStringList & djvView::toolBarLabels()
{
    static const QStringList data = QStringList() <<
        "Tool Bars" <<
        "Playback Bar" <<
        "Information Bar";

    DJV_ASSERT(data.count() == TOOL_BAR_COUNT);

    return data;
}

const QStringList & djvView::viewResizeLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "Fit Image" <<
        "Center Image";

    DJV_ASSERT(data.count() == VIEW_RESIZE_COUNT);

    return data;
}

const QStringList & djvView::gridLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "1x1" <<
        "10x10" <<
        "100x100";

    DJV_ASSERT(data.count() == GRID_COUNT);

    return data;
}

const QStringList & djvView::hudInfoLabels()
{
    static const QStringList data = QStringList() <<
        "File Name" <<
        "Layer" <<
        "Size" <<
        "Proxy" <<
        "Pixel" <<
        "Tags" <<
        "Playback Frame" <<
        "Playback Speed";

    DJV_ASSERT(data.count() == HUD_COUNT);

    return data;
}

const QStringList & djvView::hudBackgroundLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "Solid" <<
        "Shadow";

    DJV_ASSERT(data.count() == HUD_BACKGROUND_COUNT);

    return data;
}

const QStringList & djvView::imageScaleLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "16.9" <<
        "1.0" <<
        "1.33" <<
        "1.78" <<
        "1.85" <<
        "2.0" <<
        "2.35" <<
        "2.39" <<
        "2.40" <<
        "1:1"  <<
        "2:1"  <<
        "3:2"  <<
        "4:3"  <<
        "5:3"  <<
        "5:4";

    DJV_ASSERT(data.count() == IMAGE_SCALE_COUNT);

    return data;
}

djvVector2f djvView::imageScale(IMAGE_SCALE in, const djvVector2i & size)
{
    static const double data [] =
    {
        1.0,
        16.0 / 9.0,
        1.0,
        1.33,
        1.78,
        1.85,
        2.0,
        2.35,
        2.39,
        2.40,
        1.0,
        2.0,
        3.0 / 2.0,
        4.0 / 3.0,
        5.0 / 3.0,
        5.0 / 4.0
    };
    
    static const int dataCount = sizeof(data) / sizeof(data[0]);

    DJV_ASSERT(dataCount == IMAGE_SCALE_COUNT);

    djvVector2f out(1.0);

    switch (in)
    {
        case IMAGE_SCALE_NONE: break;

        default:
        
            out.x = data[in] / djvVectorUtil::aspect(size);
            
            break;
    }

    return out;
}

const QStringList & djvView::imageRotateLabels()
{
    static const QStringList data = QStringList() <<
        "0" <<
        "90" <<
        "180" <<
        "270";

    DJV_ASSERT(data.count() == IMAGE_ROTATE_COUNT);

    return data;
}

double djvView::imageRotate(IMAGE_ROTATE in)
{
    static const double data [] = { 0, 90, 180, 270 };

    return data[in];
}

void djvView::loadLut(const djvFileInfo & in, djvPixelData & lut)
{
    if (in.fileName().isEmpty())
        return;

    //DJV_DEBUG("djvView::loadLut");
    //DJV_DEBUG_PRINT("in = " << in);

    djvFileInfo file(in);

    if (file.isSequenceValid())
    {
        file.setType(djvFileInfo::SEQUENCE);
    }

    try
    {
        djvImageIoInfo info;

        QScopedPointer<djvImageLoad> load(
            djvImageIoFactory::global()->load(file, info));
    
        if (! load.data())
        {
            throw djvError(QString("Cannot open LUT \"%1\"").
                arg(file));
        }
    
        djvImage image;
        
        load->read(image);
        
        lut = image;

        //DJV_DEBUG_PRINT("lut = " << lut);
    }
    catch (const djvError & error)
    {
        DJV_APP->printError(error);
    }
}

const QStringList & djvView::playbackLabels()
{
    static const QStringList data = QStringList() <<
        "Reverse" <<
        "Stop" <<
        "Forward";

    DJV_ASSERT(data.count() == PLAYBACK_COUNT);

    return data;
}

const QStringList & djvView::frameLabels()
{
    static const QStringList data = QStringList() <<
        "Start" <<
        "Start Absolute" <<
        "Previous" <<
        "Previous X10" <<
        "Previous X100" <<
        "Next" <<
        "Next X10" <<
        "Next X100" <<
        "End" <<
        "End Absolute";
    
    DJV_ASSERT(data.count() == FRAME_COUNT);

    return data;
}

const QStringList & djvView::loopLabels()
{
    static const QStringList data = QStringList() <<
        "Once" <<
        "Repeat" <<
        "PingPong";

    DJV_ASSERT(data.count() == LOOP_COUNT);

    return data;
}

const QStringList & djvView::inOutLabels()
{
    static const QStringList data = QStringList() <<
        "Enable &In/Out Points" <<
        "&Mark In Point" <<
        "Mar&k Out Point" <<
        "Reset In Point" <<
        "Reset Out Point";

    DJV_ASSERT(data.count() == IN_OUT_COUNT);

    return data;
}

const QStringList & djvView::layoutLabels()
{
    static const QStringList data = QStringList() <<
        "Default" <<
        "Left" <<
        "Center" <<
        "Minimal";

    DJV_ASSERT(data.count() == LAYOUT_COUNT);

    return data;
}

const QStringList & djvView::histogramLabels()
{
    static const QStringList data = QStringList() <<
        "256" <<
        "1024" <<
        "2048" <<
        "4096";

    DJV_ASSERT(data.count() == HISTOGRAM_COUNT);

    return data;
}

const int djvView::histogramSize(HISTOGRAM histogram)
{
    static const QList<int> data = QList<int>() <<
        256 <<
        1024 <<
        2048 <<
        4096;
    
    DJV_ASSERT(data.count() == HISTOGRAM_COUNT);

    return data[histogram];
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvView::WINDOW_RESIZE_MAX,
    djvView::windowResizeMaxLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::TOOL_BAR, djvView::toolBarLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::VIEW_RESIZE, djvView::viewResizeLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::GRID, djvView::gridLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::HUD_BACKGROUND,
	djvView::hudBackgroundLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::IMAGE_SCALE, djvView::imageScaleLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::IMAGE_ROTATE, djvView::imageRotateLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::PLAYBACK, djvView::playbackLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::FRAME, djvView::frameLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::LOOP, djvView::loopLabels())
_DJV_STRING_OPERATOR_LABEL(djvView::LAYOUT, djvView::layoutLabels())
