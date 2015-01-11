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

//! \file djvLibquicktimeLoad.cpp

#include <djvLibquicktimeLoad.h>

#include <djvAssert.h>
#include <djvFileInfo.h>
#include <djvImage.h>
#include <djvPixelDataUtil.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvLibquicktimeLoad
//------------------------------------------------------------------------------

djvLibquicktimeLoad::djvLibquicktimeLoad(
    const djvLibquicktimePlugin::Options & options) :
    _options   (options),
    _f         (0),
    _startFrame(0),
    _frame     (0)
{}

djvLibquicktimeLoad::~djvLibquicktimeLoad()
{
    close();
}

const QStringList & djvLibquicktimeLoad::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvLibquicktimeLoad", "No video tracks in: %1");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

void djvLibquicktimeLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.

    _f = lqt_open_read(in.fileName().toLatin1().data());

    if (! _f)
    {
        throw djvError(
            djvLibquicktimePlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    if (! quicktime_has_video(_f))
    {
        throw djvError(
            djvLibquicktimePlugin::staticName,
            errorLabels()[ERROR_NO_VIDEO_TRACKS].arg(in));
    }

    if (! quicktime_supported_video(_f, 0))
    {
        throw djvError(
            djvLibquicktimePlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].arg(in));
    }

    // Get file information.

    _info.fileName = in;

    _info.size = djvVector2i(
        quicktime_video_width(_f, 0),
        quicktime_video_height(_f, 0));
    
    //DJV_DEBUG_PRINT("cmodel = " << lqt_get_cmodel(_f, 0));

    static int cmodelList [] =
    {
        BC_RGB888,
        BC_BGR888,
        BC_RGB161616,
        BC_RGBA8888,
        BC_RGBA16161616,
        LQT_COLORMODEL_NONE
    };
    
    const int cmodel = lqt_get_best_colormodel(_f, 0, cmodelList);

    switch (cmodel)
    {
        case BC_BGR888:       _info.bgr = true;
        case BC_RGB888:       _info.pixel = djvPixel::RGB_U8;   break;
        case BC_RGB161616:    _info.pixel = djvPixel::RGB_U16;  break;
        case BC_RGBA8888:     _info.pixel = djvPixel::RGBA_U8;  break;
        case BC_RGBA16161616: _info.pixel = djvPixel::RGBA_U16; break;

        throw djvError(
            djvLibquicktimePlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].arg(in));
    }

    lqt_set_cmodel(_f, 0, cmodel);

    int          timeConstant  = 0;
    const int    timeScale     = lqt_video_time_scale(_f, 0);
    const qint64 videoDuration = lqt_video_duration(_f, 0);
    const int    frameDuration = lqt_frame_duration(_f, 0, &timeConstant);
    
    //DJV_DEBUG_PRINT("time scale = " << timeScale);
    //DJV_DEBUG_PRINT("video duration = " << videoDuration);
    //DJV_DEBUG_PRINT("frame duration = " << frameDuration);
    //DJV_DEBUG_PRINT("time constant = " << timeConstant);
    //DJV_DEBUG_PRINT("start frame = " << _startFrame);

    _info.sequence = djvSequence(
        _startFrame,
        frameDuration ?
        (_startFrame + (videoDuration / frameDuration - 1)) :
        _startFrame,
        0,
        djvSpeed(timeScale, frameDuration));

    //DJV_DEBUG_PRINT("seq = " << _info.seq);

    // Get image tags.

    const QStringList & tags = djvImageTags::tagLabels();
    char * tag = 0;

    lqt_get_artist(_f);

    if (tag)
    {
        _info.tags[tags[djvImageTags::CREATOR]] = tag;
    }

    tag = lqt_get_comment(_f);

    if (tag)
    {
        _info.tags[tags[djvImageTags::DESCRIPTION]] = tag;
    }

    tag = quicktime_get_copyright(_f);

    if (tag)
    {
        _info.tags[tags[djvImageTags::COPYRIGHT]] = tag;
    }

    info = _info;
}

namespace
{

struct Rows
{
    Rows(djvPixelData * in) :
        p(new unsigned char * [in->h() + 1])
    {
        for (int y = 0; y < in->h(); ++y)
        {
            p[y] = reinterpret_cast<unsigned char *>(
                in->data(0, in->h() - 1 - y));
        }
    }

    ~Rows()
    {
        delete [] p;
    }

    unsigned char ** p;
};

} // namespace

void djvLibquicktimeLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    int _frame = frame.frame;

    if (-1 == _frame)
    {
        _frame = 0;
    }
    else
    {
        _frame -= _startFrame;
    }

    djvPixelData * data = frame.proxy ? &_tmp : &image;
    data->set(_info);

    if (_frame != this->_frame + 1)
    {
        const quint64 time = _frame * _info.sequence.speed.duration();

        //DJV_DEBUG_PRINT("time = " << static_cast<int>(time));

        lqt_seek_video(_f, 0, time);
    }

    this->_frame = _frame;

    lqt_decode_video(_f, Rows(data).p, 0);

    if (frame.proxy)
    {
        djvPixelDataInfo info = _info;
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }

    //DJV_DEBUG_PRINT("image = " << image);
}

void djvLibquicktimeLoad::close() throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeLoad::close");

    if (_f)
    {
        quicktime_close(_f);
        
        _f = 0;
    }
}

