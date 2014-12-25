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

//! \file djvViewUtil.cpp

#include <djvViewUtil.h>

#include <djvApplication.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvVectorUtil.h>

//------------------------------------------------------------------------------
// djvViewUtil
//------------------------------------------------------------------------------

djvViewUtil::~djvViewUtil()
{}

const QStringList & djvViewUtil::viewMaxLabels()
{
    static const QStringList data = QStringList() <<
        "Unlimited" <<
        "25% of screen" <<
        "50% of screen" <<
        "75% of screen" <<
        "User specified";

    DJV_ASSERT(data.count() == VIEW_MAX_COUNT);

    return data;
}

const QStringList & djvViewUtil::toolBarLabels()
{
    static const QStringList data = QStringList() <<
        "Tool Bars" <<
        "Playback Bar" <<
        "Information Bar";

    DJV_ASSERT(data.count() == TOOL_BAR_COUNT);

    return data;
}

const QStringList & djvViewUtil::gridLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "1x1" <<
        "10x10" <<
        "100x100";

    DJV_ASSERT(data.count() == GRID_COUNT);

    return data;
}

const QStringList & djvViewUtil::hudInfoLabels()
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

const QStringList & djvViewUtil::hudBackgroundLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "Solid" <<
        "Shadow";

    DJV_ASSERT(data.count() == HUD_BACKGROUND_COUNT);

    return data;
}

const QStringList & djvViewUtil::imageScaleLabels()
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

djvVector2f djvViewUtil::imageScale(IMAGE_SCALE in, const djvVector2i & size)
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

const QStringList & djvViewUtil::imageRotateLabels()
{
    static const QStringList data = QStringList() <<
        "0" <<
        "90" <<
        "180" <<
        "270";

    DJV_ASSERT(data.count() == IMAGE_ROTATE_COUNT);

    return data;
}

double djvViewUtil::imageRotate(IMAGE_ROTATE in)
{
    static const double data [] = { 0, 90, 180, 270 };

    return data[in];
}

void djvViewUtil::loadLut(const djvFileInfo & in, djvPixelData & lut)
{
    if (in.fileName().isEmpty())
        return;

    //DJV_DEBUG("djvViewUtil::loadLut");
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

const QStringList & djvViewUtil::playbackLabels()
{
    static const QStringList data = QStringList() <<
        "Reverse" <<
        "Stop" <<
        "Forward";

    DJV_ASSERT(data.count() == PLAYBACK_COUNT);

    return data;
}

const QStringList & djvViewUtil::frameLabels()
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

const QStringList & djvViewUtil::loopLabels()
{
    static const QStringList data = QStringList() <<
        "Once" <<
        "Repeat" <<
        "PingPong";

    DJV_ASSERT(data.count() == LOOP_COUNT);

    return data;
}

const QStringList & djvViewUtil::inOutLabels()
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

const QStringList & djvViewUtil::layoutLabels()
{
    static const QStringList data = QStringList() <<
        "Default" <<
        "Left" <<
        "Center" <<
        "Minimal";

    DJV_ASSERT(data.count() == LAYOUT_COUNT);

    return data;
}

const QStringList & djvViewUtil::toolLabels()
{
    static const QStringList data = QStringList() <<
        "Magnify Tool" <<
        "Color Picker Tool" <<
        "Histogram Tool" <<
        "Information Tool";

    DJV_ASSERT(data.count() == TOOL_COUNT);

    return data;
}

const QStringList & djvViewUtil::histogramLabels()
{
    static const QStringList data = QStringList() <<
        "256" <<
        "1024" <<
        "2048" <<
        "4096";

    DJV_ASSERT(data.count() == HISTOGRAM_COUNT);

    return data;
}

const int djvViewUtil::histogramSize(HISTOGRAM histogram)
{
    static const QList<int> data = QList<int>() <<
        256 <<
        1024 <<
        2048 <<
        4096;
    
    DJV_ASSERT(data.count() == HISTOGRAM_COUNT);

    return data[histogram];
}

const QStringList & djvViewUtil::shortcutLabels()
{
    static const QStringList data = QStringList() <<
    
        "Exit" <<

        "File Open" <<
        "File Reload" <<
        "File Reload Frame" <<
        "File Save" <<
        "File Save Frame" <<
        "File Close" <<
        "File Layer Default" <<
        "File Layer 1" <<
        "File Layer 2" <<
        "File Layer 3" <<
        "File Layer 4" <<
        "File Layer 5" <<
        "File Layer 6" <<
        "File Layer 7" <<
        "File Layer 8" <<
        "File Layer 9" <<
        "File Layer 10" <<
        "File Layer Previous" <<
        "File Layer Next" <<
        "File Proxy None" <<
        "File Proxy 1/2" <<
        "File Proxy 1/4" <<
        "File Proxy 1/8" <<

        "Window New" <<
        "Window Copy" <<
        "Window Close" <<
        "Window Fit" <<
        "Window Full Screen" <<
        "Window Show Controls" <<
        "Window Show Tool Bars" <<
        "Window Show Playback" <<
        "Window Show Information" <<

        "View Left" <<
        "View Right" <<
        "View Up" <<
        "View Down" <<
        "View Center" <<
        "View Zoom In" <<
        "View Zoom Out" <<
        "View Zoom Reset" <<
        "View Reset" <<
        "View Fit" <<
        "View HUD" <<

        "Image Show Frame Store" <<
        "Image Load Frame Store" <<
        "Image Mirror Horizontal" <<
        "Image Mirror Vertical" <<
        "Image Scale None" <<
        "Image Scale 16:9" <<
        "Image Scale 1.0" <<
        "Image Scale 1.33" <<
        "Image Scale 1.78" <<
        "Image Scale 1.85" <<
        "Image Scale 2.0" <<
        "Image Scale 2.35" <<
        "Image Scale 2.39" <<
        "Image Scale 2.40" <<
        "Image Scale 1:1" <<
        "Image Scale 2:1" <<
        "Image Scale 3:2" <<
        "Image Scale 4:3" <<
        "Image Scale 5:3" <<
        "Image Scale 5:4" <<
        "Image Rotate 0" <<
        "Image Rotate 90" <<
        "Image Rotate 180" <<
        "Image Rotate 270" <<
        "Image Color Profile" <<
        "Image Channel Red" <<
        "Image Channel Green" <<
        "Image Channel Blue" <<
        "Image Channel Alpha" <<
        "Image Display Profile" <<
        "Image Display Profile Reset" <<
        "Image Display Profile 1" <<
        "Image Display Profile 2" <<
        "Image Display Profile 3" <<
        "Image Display Profile 4" <<
        "Image Display Profile 5" <<
        "Image Display Profile 6" <<
        "Image Display Profile 7" <<
        "Image Display Profile 8" <<
        "Image Display Profile 9" <<
        "Image Display Profile 10" <<

        "Playback Reverse" <<
        "Playback Stop" <<
        "Playback Forward" <<
        "Playback Toggle" <<
        "Playback Loop" <<
        "Playback Start" <<
        "Playback Start Absolute" <<
        "Playback Previous" <<
        "Playback Previous 10" <<
        "Playback Previous 100" <<
        "Playback Next" <<
        "Playback Next 10" <<
        "Playback Next 100" <<
        "Playback End" <<
        "Playback End Absolute" <<
        "Playback In/Out Points" <<
        "Playback In Point Mark" <<
        "Playback In Point Reset" <<
        "Playback Out Point Mark" <<
        "Playback Out Point Reset" <<

        "Tool Magnify" <<
        "Tool Color Picker" <<
        "Tool Histogram" <<
        "Tool Information";

    DJV_ASSERT(data.count() == SHORTCUT_COUNT);

    return data;
}

const QStringList & djvViewUtil::mouseWheelLabels()
{
    static const QStringList data = QStringList() <<
        "View Zoom" <<
        "Playback Shuttle" <<
        "Playback Speed";

    DJV_ASSERT(data.count() == MOUSE_WHEEL_COUNT);

    return data;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvViewUtil::VIEW_MAX, djvViewUtil::viewMaxLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::TOOL_BAR, djvViewUtil::toolBarLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::GRID, djvViewUtil::gridLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::HUD_BACKGROUND,
    djvViewUtil::hudBackgroundLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::IMAGE_SCALE,
    djvViewUtil::imageScaleLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::IMAGE_ROTATE,
    djvViewUtil::imageRotateLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::PLAYBACK, djvViewUtil::playbackLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::FRAME, djvViewUtil::frameLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::LOOP, djvViewUtil::loopLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::LAYOUT, djvViewUtil::layoutLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::TOOL, djvViewUtil::toolLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::HISTOGRAM,
    djvViewUtil::histogramLabels())
_DJV_STRING_OPERATOR_LABEL(djvViewUtil::MOUSE_WHEEL,
    djvViewUtil::mouseWheelLabels())

