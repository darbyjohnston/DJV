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

//! \file djvImagePlay2TestImage.h

#ifndef DJV_PLAY2_TEST_IMAGE_H
#define DJV_PLAY2_TEST_IMAGE_H

#include <djvFileIo.h>
#include <djvOpenGlImage.h>
#include <djvPixelData.h>

class djvImagePlay2TestImage
{
public:

    djvImagePlay2TestImage();

    virtual ~djvImagePlay2TestImage();

    virtual void load(const QString &) throw (djvError) = 0;

    const djvPixelDataInfo & info() const;

    virtual void bind() const throw (djvError) = 0;

protected:

    void del();

    djvPixelDataInfo _info;
    GLuint           _id;
};

class djvImagePlay2TestImagePpm : public djvImagePlay2TestImage
{
public:

    djvImagePlay2TestImagePpm();

    void load(const QString &) throw (djvError);

    void bind() const throw (djvError);

private:

    djvFileIo _io;
    bool      _init;
    bool      _load;
};

#endif // DJV_PLAY2_TEST_IMAGE_H

