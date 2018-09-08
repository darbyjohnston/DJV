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

#include <ImageLoad.h>

#include <Context.h>

#include <djvErrorUtil.h>

//------------------------------------------------------------------------------
// ImageLoad
//------------------------------------------------------------------------------

ImageLoad::ImageLoad(
    Context * context,
    QObject * parent) :
    QObject(parent),
    _context(context)
{}

void ImageLoad::load(const djvFileInfo & fileInfo) throw (djvError)
{
    _load.reset(_context->imageIOFactory()->load(fileInfo, _info));
}

const djvImageIOInfo & ImageLoad::info() const
{
    return _info;
}

const djvImage * ImageLoad::image(qint64 frame) const
{
    //DJV_DEBUG("ImageLoad::image");
    //DJV_DEBUG_PRINT("frame = " << frame);
    try
    {
        ImageLoad * that = const_cast<ImageLoad  *>(this);
        _load->read(that->_image, frame);
        //DJV_DEBUG_PRINT("image = " << _image);
    }
    catch (const djvError &)
    {
        //DJV_DEBUG_PRINT("error = " << djvErrorUtil::format(error));
    }
    return &_image;
}
