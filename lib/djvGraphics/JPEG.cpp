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

#include <djvGraphics/JPEG.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

using namespace djv;

namespace djv
{
    namespace Graphics
    {
        JPEG::Options::Options() :
            quality(90)
        {}

        const QString JPEG::staticName = "JPEG";

        const QStringList & JPEG::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::JPEG", "Quality");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace Graphics
} // namespace djv

extern "C"
{
    void djvJPEGError(libjpeg::j_common_ptr in)
    {
        Graphics::JPEGErrorStruct * error = (Graphics::JPEGErrorStruct *)in->err;
        in->err->format_message(in, error->msg);
        ::longjmp(error->jump, 1);
    }

    void djvJPEGWarning(libjpeg::j_common_ptr in, int level)
    {
        if (level > 0)
            return;
        Graphics::JPEGErrorStruct * error = (Graphics::JPEGErrorStruct *)in->err;
        in->err->format_message(in, error->msg);
        ::longjmp(error->jump, 1);
    }

} // extern "C"
