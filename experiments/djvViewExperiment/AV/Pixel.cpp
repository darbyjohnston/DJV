//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <AV/Pixel.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        QString Pixel::getPixelLabel(const Pixel& pixel)
        {
            static const std::map<GLenum, QString> formatLabel =
            {
                { GL_RED,  qApp->translate("djv::AV::Pixel", "L") },
                { GL_RG,   qApp->translate("djv::AV::Pixel", "LA") },
                { GL_RGB,  qApp->translate("djv::AV::Pixel", "RGB") },
                { GL_RGBA, qApp->translate("djv::AV::Pixel", "RGBA") }
            };
            static const std::map<GLenum, QString> typeLabel =
            {
                { GL_UNSIGNED_BYTE,           qApp->translate("djv::AV::Pixel", "U8") },
                { GL_UNSIGNED_INT_10_10_10_2, qApp->translate("djv::AV::Pixel", "U10") },
                { GL_UNSIGNED_SHORT,          qApp->translate("djv::AV::Pixel", "U16") },
                { GL_UNSIGNED_INT,            qApp->translate("djv::AV::Pixel", "U32") },
                { GL_HALF_FLOAT,              qApp->translate("djv::AV::Pixel", "F16") },
                { GL_FLOAT,                   qApp->translate("djv::AV::Pixel", "F32") }
            };
            const auto i = formatLabel.find(pixel._format);
            const auto j = typeLabel.find(pixel._type);
            return i != formatLabel.end() && j != typeLabel.end() ?
                (i->second + " " + j->second) :
                qApp->translate("djv::AV::Pixel", "None");
        }

    } // namespace AV
} // namespace djv

