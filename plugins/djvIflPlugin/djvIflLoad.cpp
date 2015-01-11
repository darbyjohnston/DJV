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

//! \file djvIflLoad.cpp

#include <djvIflLoad.h>

#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvFileInfoUtil.h>
#include <djvImage.h>
#include <djvPixel.h>

//------------------------------------------------------------------------------
// djvIflLoad
//------------------------------------------------------------------------------

djvIflLoad::~djvIflLoad()
{}

void djvIflLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvIflLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    _list.clear();

    QStringList tmp;

    try
    {
        tmp = djvFileIoUtil::lines(in);

        //DJV_DEBUG_PRINT("list = " << tmp);
    }
    catch (const djvError &)
    {
        throw djvError(
            djvIflPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_READ].arg(in));
    }

    for (int i = 0; i < tmp.count(); ++i)
    {
        if (tmp[i].isEmpty())
        {
            continue;
        }

        djvFileInfo file(tmp[i]);

        if (file.path().isEmpty())
        {
            file.setPath(in.path());
        }

        if (file.isSequenceValid())
        {
            file.setType(djvFileInfo::SEQUENCE);

            _list += djvFileInfoUtil::expandSequence(file);
        }
        else
        {
            _list += tmp[i];
        }
    }

    //DJV_DEBUG_PRINT("list = " << _list);

    QScopedPointer<djvImageLoad> plugin(djvImageIoFactory::global()->load(
        _list.count() ? _list[0] : QString(), info));

    info.sequence.frames.resize(_list.count());

    for (int i = 0; i < _list.count(); ++i)
    {
        info.sequence.frames[i] = i;
    }
}

void djvIflLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvIflLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    QString fileName;

    if (_list.count())
    {
        if (-1 == frame.frame)
        {
            fileName = _list[0];
        }
        else if (
            frame.frame >= 0 &&
            frame.frame < _list.count())
        {
            fileName = _list[frame.frame];
        }
    }

    //DJV_DEBUG_PRINT("file name = " << fileName);

    djvImageIoInfo info;

    QScopedPointer<djvImageLoad> load(
        djvImageIoFactory::global()->load(fileName, info));

    load->read(image, djvImageIoFrameInfo(-1, frame.layer, frame.proxy));
}
