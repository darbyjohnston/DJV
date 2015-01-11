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

//! \file djvLibquicktimeSave.cpp

#include <djvLibquicktimeSave.h>

#include <djvFileInfo.h>
#include <djvOpenGlImage.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvLibquicktimeSave
//------------------------------------------------------------------------------

djvLibquicktimeSave::djvLibquicktimeSave(
    const djvLibquicktimePlugin::Options & options) :
    _options      (options),
    _f            (0),
    _frame        (0),
    _frameDuration(0)
{}

djvLibquicktimeSave::~djvLibquicktimeSave()
{}

const QStringList & djvLibquicktimeSave::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvLibquicktimeSave", "Cannot set codec for: %1") <<
        qApp->translate("djvLibquicktimeSave", "Cannot set color model for: %1");
    
    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}

namespace
{

struct CodecInfo
{
    CodecInfo(const QString & name) :
        p(lqt_find_video_codec_by_name(name.toLatin1().data()))
    {}

    ~CodecInfo()
    {
        if (p)
        {
            lqt_destroy_codec_info(p);
        }
    }

    lqt_codec_info_t ** p;
};

} // namespace

void djvLibquicktimeSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeSave::open");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("info = " << info);

    // Open the file.

    _f = lqt_open_write(in.fileName().toLatin1().data(), LQT_FILE_QT);

    if (! _f)
    {
        throw djvError(
            djvLibquicktimePlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_OPEN].arg(in));
    }

    CodecInfo codecInfo(_options.codec);

    if (! codecInfo.p)
    {
        throw djvError(
            djvLibquicktimePlugin::staticName,
            errorLabels()[ERROR_SET_CODEC].arg(in));
    }

    _frame = 0;
    _frameDuration = info.sequence.speed.duration();

    //! \todo Should a return value be handled here?

    lqt_set_video(
        _f,
        1,
        info.size.x,
        info.size.y,
        _frameDuration,
        info.sequence.speed.scale(),
        codecInfo.p[0]);

    _info          = djvPixelDataInfo();
    _info.fileName = in;
    _info.size     = info.size;
    _info.pixel    = info.pixel;

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

        default:
        
            throw djvError(
                djvLibquicktimePlugin::staticName,
                errorLabels()[ERROR_SET_COLOR_MODEL].arg(in));
    }

    lqt_set_cmodel(_f, 0, cmodel);

    //DJV_DEBUG_PRINT("info = " << _info);

    const QStringList & tags = djvImageTags::tagLabels();
    QString tag;

    tag = info.tags[tags[djvImageTags::CREATOR]];

    if (tag.length())
    {
        lqt_set_artist(_f, (char *)tag.toLatin1().data());
    }

    tag = info.tags[tags[djvImageTags::DESCRIPTION]];

    if (tag.length())
    {
        lqt_set_comment(_f, (char *)tag.toLatin1().data());
    }

    tag = info.tags[tags[djvImageTags::COPYRIGHT]];

    if (tag.length())
    {
        quicktime_set_copyright(_f, (char *)tag.toLatin1().data());
    }

    _image.set(_info);
}

namespace
{

// Convert our image data to an array of row pointers for libquicktime.

struct Rows
{
    Rows(const djvPixelData * in) :
        p(new const unsigned char * [in->h() + 1])
    {
        for (int y = 0; y < in->h(); ++y)
        {
            p[y] = in->data(0, in->h() - 1 - y);
        }
    }

    ~Rows()
    {
        delete [] p;
    }

    const unsigned char ** p;
};

} // namespace

void djvLibquicktimeSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeSave::write");
    //DJV_DEBUG_PRINT("in = " << in);

    // Convert the image if necessary.

    const djvPixelData * p = &in;

    if (in.info() != _info)
    {
        //DJV_DEBUG_PRINT("convert = " << _image);

        _image.zero();

        djvOpenGlImage::copy(in, _image);

        p = &_image;
    }

    // Write the image.

    lqt_encode_video(
        _f,
        const_cast<unsigned char **>(Rows(p).p),
        0,
        _frame * _frameDuration);

    ++_frame;
}

void djvLibquicktimeSave::close() throw (djvError)
{
    //DJV_DEBUG("djvLibquicktimeSave::close");

    if (_f)
    {
        quicktime_close(_f);
        
        _f = 0;
    }
}

