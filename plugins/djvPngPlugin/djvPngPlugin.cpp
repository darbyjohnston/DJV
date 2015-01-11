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

//! \file djvPngPlugin.cpp

#include <djvPngPlugin.h>

#include <djvPngLoad.h>
#include <djvPngSave.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvPngPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvPngPlugin
//------------------------------------------------------------------------------

const QString djvPngPlugin::staticName = "PNG";

djvPlugin * djvPngPlugin::copyPlugin() const
{
    return new djvPngPlugin;
}

QString djvPngPlugin::pluginName() const
{
    return staticName;
}

QStringList djvPngPlugin::extensions() const
{
    return QStringList() << ".png";
}

djvImageLoad * djvPngPlugin::createLoad() const
{
    return new djvPngLoad;
}

djvImageSave * djvPngPlugin::createSave() const
{
    return new djvPngSave;
}

//------------------------------------------------------------------------------

extern "C"
{

void djvPngError(png_structp in, png_const_charp msg)
{
    djvPngErrorStruct * error = (djvPngErrorStruct *)png_get_error_ptr(in);

    SNPRINTF(error->msg, djvStringUtil::cStringLength, "%s", msg);

    longjmp(png_jmpbuf(in), 1);
}

void djvPngWarning(png_structp in, png_const_charp msg)
{
    djvPngErrorStruct * error = (djvPngErrorStruct *)png_get_error_ptr(in);

    SNPRINTF(error->msg, djvStringUtil::cStringLength, "%s", msg);

    longjmp(png_jmpbuf(in), 1);
}

} // extern "C"

