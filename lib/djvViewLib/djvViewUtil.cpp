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

//! \file djvViewUtil.cpp

#include <djvViewUtil.h>

#include <djvViewApplication.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvImageIo.h>
#include <djvVectorUtil.h>

#include <QDir>

//------------------------------------------------------------------------------
// djvViewUtil
//------------------------------------------------------------------------------

djvViewUtil::~djvViewUtil()
{}

const QStringList & djvViewUtil::viewMaxLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Unlimited") <<
        qApp->translate("djvViewUtil", "25% of screen") <<
        qApp->translate("djvViewUtil", "50% of screen") <<
        qApp->translate("djvViewUtil", "75% of screen") <<
        qApp->translate("djvViewUtil", "User specified");

    DJV_ASSERT(data.count() == VIEW_MAX_COUNT);

    return data;
}

const QStringList & djvViewUtil::toolBarLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Tool Bars") <<
        qApp->translate("djvViewUtil", "Playback Bar") <<
        qApp->translate("djvViewUtil", "Information Bar");

    DJV_ASSERT(data.count() == TOOL_BAR_COUNT);

    return data;
}

const QStringList & djvViewUtil::gridLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "None") <<
        qApp->translate("djvViewUtil", "1x1") <<
        qApp->translate("djvViewUtil", "10x10") <<
        qApp->translate("djvViewUtil", "100x100");

    DJV_ASSERT(data.count() == GRID_COUNT);

    return data;
}

const QStringList & djvViewUtil::hudInfoLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "File Name") <<
        qApp->translate("djvViewUtil", "Layer") <<
        qApp->translate("djvViewUtil", "Size") <<
        qApp->translate("djvViewUtil", "Proxy") <<
        qApp->translate("djvViewUtil", "Pixel") <<
        qApp->translate("djvViewUtil", "Tags") <<
        qApp->translate("djvViewUtil", "Playback Frame") <<
        qApp->translate("djvViewUtil", "Playback Speed");

    DJV_ASSERT(data.count() == HUD_COUNT);

    return data;
}

const QStringList & djvViewUtil::hudBackgroundLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "None") <<
        qApp->translate("djvViewUtil", "Solid") <<
        qApp->translate("djvViewUtil", "Shadow");

    DJV_ASSERT(data.count() == HUD_BACKGROUND_COUNT);

    return data;
}

const QStringList & djvViewUtil::imageScaleLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "None") <<
        qApp->translate("djvViewUtil", "16.9") <<
        qApp->translate("djvViewUtil", "1.0") <<
        qApp->translate("djvViewUtil", "1.33") <<
        qApp->translate("djvViewUtil", "1.78") <<
        qApp->translate("djvViewUtil", "1.85") <<
        qApp->translate("djvViewUtil", "2.0") <<
        qApp->translate("djvViewUtil", "2.35") <<
        qApp->translate("djvViewUtil", "2.39") <<
        qApp->translate("djvViewUtil", "2.40") <<
        qApp->translate("djvViewUtil", "1:1") <<
        qApp->translate("djvViewUtil", "2:1") <<
        qApp->translate("djvViewUtil", "3:2") <<
        qApp->translate("djvViewUtil", "4:3") <<
        qApp->translate("djvViewUtil", "5:3") <<
        qApp->translate("djvViewUtil", "5:4");

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
        qApp->translate("djvViewUtil", "0") <<
        qApp->translate("djvViewUtil", "90") <<
        qApp->translate("djvViewUtil", "180") <<
        qApp->translate("djvViewUtil", "270");

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
    
        djvImage image;
        
        load->read(image);
        
        lut = image;

        //DJV_DEBUG_PRINT("lut = " << lut);
    }
    catch (djvError error)
    {
        error.add(
            errorLabels()[ERROR_OPEN_LUT].
            arg(QDir::toNativeSeparators(file)));

        DJV_VIEW_APP->printError(error);
    }
}

const QStringList & djvViewUtil::playbackLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Reverse") <<
        qApp->translate("djvViewUtil", "Stop") <<
        qApp->translate("djvViewUtil", "Forward");

    DJV_ASSERT(data.count() == PLAYBACK_COUNT);

    return data;
}

const QStringList & djvViewUtil::frameLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Start") <<
        qApp->translate("djvViewUtil", "Start Absolute") <<
        qApp->translate("djvViewUtil", "Previous") <<
        qApp->translate("djvViewUtil", "Previous X10") <<
        qApp->translate("djvViewUtil", "Previous X100") <<
        qApp->translate("djvViewUtil", "Next") <<
        qApp->translate("djvViewUtil", "Next X10") <<
        qApp->translate("djvViewUtil", "Next X100") <<
        qApp->translate("djvViewUtil", "End") <<
        qApp->translate("djvViewUtil", "End Absolute");
    
    DJV_ASSERT(data.count() == FRAME_COUNT);

    return data;
}

const QStringList & djvViewUtil::loopLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Once") <<
        qApp->translate("djvViewUtil", "Repeat") <<
        qApp->translate("djvViewUtil", "PingPong");

    DJV_ASSERT(data.count() == LOOP_COUNT);

    return data;
}

const QStringList & djvViewUtil::inOutLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Enable &In/Out Points") <<
        qApp->translate("djvViewUtil", "&Mark In Point") <<
        qApp->translate("djvViewUtil", "Mar&k Out Point") <<
        qApp->translate("djvViewUtil", "Reset In Point") <<
        qApp->translate("djvViewUtil", "Reset Out Point");

    DJV_ASSERT(data.count() == IN_OUT_COUNT);

    return data;
}

const QStringList & djvViewUtil::layoutLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Default") <<
        qApp->translate("djvViewUtil", "Left") <<
        qApp->translate("djvViewUtil", "Center") <<
        qApp->translate("djvViewUtil", "Minimal");

    DJV_ASSERT(data.count() == LAYOUT_COUNT);

    return data;
}

const QStringList & djvViewUtil::toolLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Magnify Tool") <<
        qApp->translate("djvViewUtil", "Color Picker Tool") <<
        qApp->translate("djvViewUtil", "Histogram Tool") <<
        qApp->translate("djvViewUtil", "Information Tool");

    DJV_ASSERT(data.count() == TOOL_COUNT);

    return data;
}

const QStringList & djvViewUtil::histogramLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "256") <<
        qApp->translate("djvViewUtil", "1024") <<
        qApp->translate("djvViewUtil", "2048") <<
        qApp->translate("djvViewUtil", "4096");

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
    
        qApp->translate("djvViewUtil", "Exit") <<

        qApp->translate("djvViewUtil", "File Open") <<
        qApp->translate("djvViewUtil", "File Reload") <<
        qApp->translate("djvViewUtil", "File Reload Frame") <<
        qApp->translate("djvViewUtil", "File Save") <<
        qApp->translate("djvViewUtil", "File Save Frame") <<
        qApp->translate("djvViewUtil", "File Close") <<
        qApp->translate("djvViewUtil", "File Layer Default") <<
        qApp->translate("djvViewUtil", "File Layer 1") <<
        qApp->translate("djvViewUtil", "File Layer 2") <<
        qApp->translate("djvViewUtil", "File Layer 3") <<
        qApp->translate("djvViewUtil", "File Layer 4") <<
        qApp->translate("djvViewUtil", "File Layer 5") <<
        qApp->translate("djvViewUtil", "File Layer 6") <<
        qApp->translate("djvViewUtil", "File Layer 7") <<
        qApp->translate("djvViewUtil", "File Layer 8") <<
        qApp->translate("djvViewUtil", "File Layer 9") <<
        qApp->translate("djvViewUtil", "File Layer 10") <<
        qApp->translate("djvViewUtil", "File Layer Previous") <<
        qApp->translate("djvViewUtil", "File Layer Next") <<
        qApp->translate("djvViewUtil", "File Proxy None") <<
        qApp->translate("djvViewUtil", "File Proxy 1/2") <<
        qApp->translate("djvViewUtil", "File Proxy 1/4") <<
        qApp->translate("djvViewUtil", "File Proxy 1/8") <<

        qApp->translate("djvViewUtil", "Window New") <<
        qApp->translate("djvViewUtil", "Window Copy") <<
        qApp->translate("djvViewUtil", "Window Close") <<
        qApp->translate("djvViewUtil", "Window Fit") <<
        qApp->translate("djvViewUtil", "Window Full Screen") <<
        qApp->translate("djvViewUtil", "Window Show Controls") <<
        qApp->translate("djvViewUtil", "Window Show Tool Bars") <<
        qApp->translate("djvViewUtil", "Window Show Playback") <<
        qApp->translate("djvViewUtil", "Window Show Information") <<

        qApp->translate("djvViewUtil", "View Left") <<
        qApp->translate("djvViewUtil", "View Right") <<
        qApp->translate("djvViewUtil", "View Up") <<
        qApp->translate("djvViewUtil", "View Down") <<
        qApp->translate("djvViewUtil", "View Center") <<
        qApp->translate("djvViewUtil", "View Zoom In") <<
        qApp->translate("djvViewUtil", "View Zoom Out") <<
        qApp->translate("djvViewUtil", "View Zoom Reset") <<
        qApp->translate("djvViewUtil", "View Reset") <<
        qApp->translate("djvViewUtil", "View Fit") <<
        qApp->translate("djvViewUtil", "View HUD") <<

        qApp->translate("djvViewUtil", "Image Show Frame Store") <<
        qApp->translate("djvViewUtil", "Image Load Frame Store") <<
        qApp->translate("djvViewUtil", "Image Mirror Horizontal") <<
        qApp->translate("djvViewUtil", "Image Mirror Vertical") <<
        qApp->translate("djvViewUtil", "Image Scale None") <<
        qApp->translate("djvViewUtil", "Image Scale 16:9") <<
        qApp->translate("djvViewUtil", "Image Scale 1.0") <<
        qApp->translate("djvViewUtil", "Image Scale 1.33") <<
        qApp->translate("djvViewUtil", "Image Scale 1.78") <<
        qApp->translate("djvViewUtil", "Image Scale 1.85") <<
        qApp->translate("djvViewUtil", "Image Scale 2.0") <<
        qApp->translate("djvViewUtil", "Image Scale 2.35") <<
        qApp->translate("djvViewUtil", "Image Scale 2.39") <<
        qApp->translate("djvViewUtil", "Image Scale 2.40") <<
        qApp->translate("djvViewUtil", "Image Scale 1:1") <<
        qApp->translate("djvViewUtil", "Image Scale 2:1") <<
        qApp->translate("djvViewUtil", "Image Scale 3:2") <<
        qApp->translate("djvViewUtil", "Image Scale 4:3") <<
        qApp->translate("djvViewUtil", "Image Scale 5:3") <<
        qApp->translate("djvViewUtil", "Image Scale 5:4") <<
        qApp->translate("djvViewUtil", "Image Rotate 0") <<
        qApp->translate("djvViewUtil", "Image Rotate 90") <<
        qApp->translate("djvViewUtil", "Image Rotate 180") <<
        qApp->translate("djvViewUtil", "Image Rotate 270") <<
        qApp->translate("djvViewUtil", "Image Color Profile") <<
        qApp->translate("djvViewUtil", "Image Channel Red") <<
        qApp->translate("djvViewUtil", "Image Channel Green") <<
        qApp->translate("djvViewUtil", "Image Channel Blue") <<
        qApp->translate("djvViewUtil", "Image Channel Alpha") <<
        qApp->translate("djvViewUtil", "Image Display Profile") <<
        qApp->translate("djvViewUtil", "Image Display Profile Reset") <<
        qApp->translate("djvViewUtil", "Image Display Profile 1") <<
        qApp->translate("djvViewUtil", "Image Display Profile 2") <<
        qApp->translate("djvViewUtil", "Image Display Profile 3") <<
        qApp->translate("djvViewUtil", "Image Display Profile 4") <<
        qApp->translate("djvViewUtil", "Image Display Profile 5") <<
        qApp->translate("djvViewUtil", "Image Display Profile 6") <<
        qApp->translate("djvViewUtil", "Image Display Profile 7") <<
        qApp->translate("djvViewUtil", "Image Display Profile 8") <<
        qApp->translate("djvViewUtil", "Image Display Profile 9") <<
        qApp->translate("djvViewUtil", "Image Display Profile 10") <<

        qApp->translate("djvViewUtil", "Playback Reverse") <<
        qApp->translate("djvViewUtil", "Playback Stop") <<
        qApp->translate("djvViewUtil", "Playback Forward") <<
        qApp->translate("djvViewUtil", "Playback Toggle") <<
        qApp->translate("djvViewUtil", "Playback Loop") <<
        qApp->translate("djvViewUtil", "Playback Start") <<
        qApp->translate("djvViewUtil", "Playback Start Absolute") <<
        qApp->translate("djvViewUtil", "Playback Previous") <<
        qApp->translate("djvViewUtil", "Playback Previous 10") <<
        qApp->translate("djvViewUtil", "Playback Previous 100") <<
        qApp->translate("djvViewUtil", "Playback Next") <<
        qApp->translate("djvViewUtil", "Playback Next 10") <<
        qApp->translate("djvViewUtil", "Playback Next 100") <<
        qApp->translate("djvViewUtil", "Playback End") <<
        qApp->translate("djvViewUtil", "Playback End Absolute") <<
        qApp->translate("djvViewUtil", "Playback In/Out Points") <<
        qApp->translate("djvViewUtil", "Playback In Point Mark") <<
        qApp->translate("djvViewUtil", "Playback In Point Reset") <<
        qApp->translate("djvViewUtil", "Playback Out Point Mark") <<
        qApp->translate("djvViewUtil", "Playback Out Point Reset") <<

        qApp->translate("djvViewUtil", "Tool Magnify") <<
        qApp->translate("djvViewUtil", "Tool Color Picker") <<
        qApp->translate("djvViewUtil", "Tool Histogram") <<
        qApp->translate("djvViewUtil", "Tool Information");

    DJV_ASSERT(data.count() == SHORTCUT_COUNT);

    return data;
}

const QStringList & djvViewUtil::mouseWheelLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "View Zoom") <<
        qApp->translate("djvViewUtil", "Playback Shuttle") <<
        qApp->translate("djvViewUtil", "Playback Speed");

    DJV_ASSERT(data.count() == MOUSE_WHEEL_COUNT);

    return data;
}

const QStringList & djvViewUtil::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvViewUtil", "Cannot open image: \"%1\"") <<
        qApp->translate("djvViewUtil", "Cannot read image: \"%1\"") <<
        qApp->translate("djvViewUtil", "Cannot write image: \"%1\"") <<
        qApp->translate("djvViewUtil", "Cannot open LUT: \"%1\"") <<
        qApp->translate("djvViewUtil", "Cannot pick color") <<
        qApp->translate("djvViewUtil", "Cannot compute histogram") <<
        qApp->translate("djvViewUtil", "Cannot magnify");

    DJV_ASSERT(ERROR_COUNT == data.count());
    
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

