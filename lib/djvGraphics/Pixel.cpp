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

#include <djvGraphics/Pixel.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        Pixel::~Pixel()
        {}

        const QStringList & Pixel::formatLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::Pixel", "L") <<
                qApp->translate("djv::Graphics::Pixel", "LA") <<
                qApp->translate("djv::Graphics::Pixel", "RGB") <<
                qApp->translate("djv::Graphics::Pixel", "RGBA");
            DJV_ASSERT(data.count() == FORMAT_COUNT);
            return data;
        }

        const QStringList & Pixel::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::Pixel", "U8") <<
                qApp->translate("djv::Graphics::Pixel", "U10") <<
                qApp->translate("djv::Graphics::Pixel", "U16") <<
                qApp->translate("djv::Graphics::Pixel", "F16") <<
                qApp->translate("djv::Graphics::Pixel", "F32");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        const QStringList & Pixel::dataLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::Pixel", "Integer") <<
                qApp->translate("djv::Graphics::Pixel", "Float");
            DJV_ASSERT(data.count() == DATA_COUNT);
            return data;
        }

        const QStringList & Pixel::pixelLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::Pixel", "L U8") <<
                qApp->translate("djv::Graphics::Pixel", "L U16") <<
                qApp->translate("djv::Graphics::Pixel", "L F16") <<
                qApp->translate("djv::Graphics::Pixel", "L F32") <<
                qApp->translate("djv::Graphics::Pixel", "LA U8") <<
                qApp->translate("djv::Graphics::Pixel", "LA U16") <<
                qApp->translate("djv::Graphics::Pixel", "LA F16") <<
                qApp->translate("djv::Graphics::Pixel", "LA F32") <<
                qApp->translate("djv::Graphics::Pixel", "RGB U8") <<
                qApp->translate("djv::Graphics::Pixel", "RGB U10") <<
                qApp->translate("djv::Graphics::Pixel", "RGB U16") <<
                qApp->translate("djv::Graphics::Pixel", "RGB F16") <<
                qApp->translate("djv::Graphics::Pixel", "RGB F32") <<
                qApp->translate("djv::Graphics::Pixel", "RGBA U8") <<
                qApp->translate("djv::Graphics::Pixel", "RGBA U16") <<
                qApp->translate("djv::Graphics::Pixel", "RGBA F16") <<
                qApp->translate("djv::Graphics::Pixel", "RGBA F32");
            DJV_ASSERT(data.count() == PIXEL_COUNT);
            return data;
        }

    } // namespace Graphics

    _DJV_STRING_OPERATOR_LABEL(Graphics::Pixel::FORMAT, Graphics::Pixel::formatLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::Pixel::TYPE, Graphics::Pixel::typeLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::Pixel::DATA, Graphics::Pixel::dataLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::Pixel::PIXEL, Graphics::Pixel::pixelLabels());

    QStringList & operator >> (QStringList & in, Graphics::Pixel::Mask & out) throw (QString)
    {
        for (int i = 0; i < Graphics::Pixel::channelsMax; ++i)
        {
            in >> out[i];
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const Graphics::Pixel::Mask & in)
    {
        for (int i = 0; i < Graphics::Pixel::channelsMax; ++i)
        {
            out << in[i];
        }
        return out;
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::Pixel::FORMAT & in)
    {
        return debug << Core::StringUtil::label(in);
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::Pixel::TYPE & in)
    {
        return debug << Core::StringUtil::label(in);
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::Pixel::DATA & in)
    {
        return debug << Core::StringUtil::label(in);
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::Pixel::PIXEL & in)
    {
        return debug << Core::StringUtil::label(in);
    }

    Core::Debug & operator << (Core::Debug & debug, const Graphics::Pixel::Mask & in)
    {
        for (int i = 0; i < Graphics::Pixel::channelsMax; ++i)
        {
            debug << static_cast<int>(in[i]);
        }
        return debug;
    }

} // namespace djv
