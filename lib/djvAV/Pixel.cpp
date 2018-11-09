//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvAV/Pixel.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        Pixel::~Pixel()
        {}

        const QStringList & Pixel::formatLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Pixel", "L") <<
                qApp->translate("djv::AV::Pixel", "LA") <<
                qApp->translate("djv::AV::Pixel", "RGB") <<
                qApp->translate("djv::AV::Pixel", "RGBA");
            DJV_ASSERT(data.count() == FORMAT_COUNT);
            return data;
        }

        const QStringList & Pixel::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Pixel", "U8") <<
                qApp->translate("djv::AV::Pixel", "U10") <<
                qApp->translate("djv::AV::Pixel", "U16") <<
                qApp->translate("djv::AV::Pixel", "F16") <<
                qApp->translate("djv::AV::Pixel", "F32");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        const QStringList & Pixel::dataLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Pixel", "Integer") <<
                qApp->translate("djv::AV::Pixel", "Float");
            DJV_ASSERT(data.count() == DATA_COUNT);
            return data;
        }

        const QStringList & Pixel::pixelLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Pixel", "L U8") <<
                qApp->translate("djv::AV::Pixel", "L U16") <<
                qApp->translate("djv::AV::Pixel", "L F16") <<
                qApp->translate("djv::AV::Pixel", "L F32") <<
                qApp->translate("djv::AV::Pixel", "LA U8") <<
                qApp->translate("djv::AV::Pixel", "LA U16") <<
                qApp->translate("djv::AV::Pixel", "LA F16") <<
                qApp->translate("djv::AV::Pixel", "LA F32") <<
                qApp->translate("djv::AV::Pixel", "RGB U8") <<
                qApp->translate("djv::AV::Pixel", "RGB U10") <<
                qApp->translate("djv::AV::Pixel", "RGB U16") <<
                qApp->translate("djv::AV::Pixel", "RGB F16") <<
                qApp->translate("djv::AV::Pixel", "RGB F32") <<
                qApp->translate("djv::AV::Pixel", "RGBA U8") <<
                qApp->translate("djv::AV::Pixel", "RGBA U16") <<
                qApp->translate("djv::AV::Pixel", "RGBA F16") <<
                qApp->translate("djv::AV::Pixel", "RGBA F32");
            DJV_ASSERT(data.count() == PIXEL_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::Pixel::FORMAT, AV::Pixel::formatLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::Pixel::TYPE, AV::Pixel::typeLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::Pixel::DATA, AV::Pixel::dataLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::Pixel::PIXEL, AV::Pixel::pixelLabels());

    QStringList & operator >> (QStringList & in, AV::Pixel::Mask & out)
    {
        for (int i = 0; i < AV::Pixel::channelsMax; ++i)
        {
            in >> out[i];
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const AV::Pixel::Mask & in)
    {
        for (int i = 0; i < AV::Pixel::channelsMax; ++i)
        {
            out << in[i];
        }
        return out;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Pixel::FORMAT & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Pixel::TYPE & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Pixel::DATA & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Pixel::PIXEL & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

    Core::Debug & operator << (Core::Debug & debug, const AV::Pixel::Mask & in)
    {
        for (int i = 0; i < AV::Pixel::channelsMax; ++i)
        {
            debug << static_cast<int>(in[i]);
        }
        return debug;
    }

} // namespace djv
