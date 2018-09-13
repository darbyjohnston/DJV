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

#include <djvGraphics/LUT.h>

#include <djvGraphics/ImageIO.h>

#include <djvCore/Assert.h>
#include <djvGraphics/Color.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileIOUtil.h>

#include <QCoreApplication>
#include <QRegExp>

#include <stdio.h>

using namespace djv;

namespace djv
{
    namespace Graphics
    {
        LUT::Options::Options() :
            type(TYPE_AUTO)
        {}

        const QString LUT::staticName = "LUT";

        const QStringList LUT::staticExtensions = QStringList() <<
            ".lut" <<
            ".1dl";

        const QStringList & LUT::formatLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::LUT", "Inferno") <<
                qApp->translate("djv::Graphics::LUT", "Kodak");
            DJV_ASSERT(data.count() == FORMAT_COUNT);
            return data;
        }

        const QStringList & LUT::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::LUT", "Auto") <<
                qApp->translate("djv::Graphics::LUT", "U8") <<
                qApp->translate("djv::Graphics::LUT", "U10") <<
                qApp->translate("djv::Graphics::LUT", "U16");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        namespace
        {
            int _bitDepth(djvFileIO & io) throw (djvError)
            {
                int out = 0;
                const quint64 pos = io.pos();
                char tmp[djvStringUtil::cStringLength] = "";
                while (1)
                {
                    try
                    {
                        djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
                    }
                    catch (const djvError &)
                    {
                        break;
                    }
                    out = djvMath::max(QString(tmp).toInt(), out);
                }
                io.setPos(pos);
                if (out <= Pixel::u8Max)
                {
                    return 8;
                }
                else if (out <= Pixel::u10Max)
                {
                    return 10;
                }
                return 16;
            }

        } // namespace

        void LUT::infernoOpen(djvFileIO & io, PixelDataInfo & info, TYPE type)
            throw (djvError)
        {
            //DJV_DEBUG("infernoOpen");

            // Header.
            char tmp[djvStringUtil::cStringLength] = "";
            djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
            //DJV_DEBUG_PRINT("magic = " << tmp);
            if (QString(tmp) != "LUT:")
            {
                throw djvError(
                    LUT::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNRECOGNIZED]);
            }
            djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
            const int channels = QString(tmp).toInt();
            djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
            const int size = QString(tmp).toInt();
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("channels = " << channels);

            // Information.
            info.size = glm::ivec2(size, 1);
            int bitDepth = 0;
            switch (type)
            {
            case TYPE_AUTO: bitDepth = _bitDepth(io); break;
            case TYPE_U8:   bitDepth = 8;             break;
            case TYPE_U10:  bitDepth = 10;            break;
            case TYPE_U16:  bitDepth = 16;            break;
            default: break;
            }
            //DJV_DEBUG_PRINT("bit depth = " << bitDepth);
            if (!Pixel::pixel(channels, bitDepth, Pixel::INTEGER, info.pixel))
            {
                throw djvError(
                    LUT::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
        }

        void LUT::infernoOpen(djvFileIO & io, const PixelDataInfo & info)
            throw (djvError)
        {
            char tmp[djvStringUtil::cStringLength] = "";
            int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "LUT: %d %d\n\n",
                Pixel::channels(info.pixel), info.size.x);
            io.set(tmp, size);
        }

        void LUT::kodakOpen(djvFileIO & io, PixelDataInfo & info, TYPE type)
            throw (djvError)
        {
            //DJV_DEBUG("LUT::kodakOpen");

            // Header.
            const quint64 pos = io.pos();
            QString header;
            qint8 c = 0;
            bool comment = false;
            while (1)
            {
                try
                {
                    io.get8(&c);
                }
                catch (const djvError &)
                {
                    break;
                }
                if ('#' == c)
                {
                    comment = true;
                }
                else if ('\n' == c)
                {
                    if (comment)
                    {
                        comment = false;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (!comment)
                    {
                        header += c;
                    }
                }
            }
            //DJV_DEBUG_PRINT("header = " << header);

            const QStringList split = header.split(
                QRegExp("\\s"),
                QString::SkipEmptyParts);
            //DJV_DEBUG_PRINT("split = " << split.join("|"));

            const int channels = split.count();
            //DJV_DEBUG_PRINT("channels = " << channels);

            int size = 1;
            while (1)
            {
                try
                {
                    io.get8(&c);
                }
                catch (const djvError &)
                {
                    break;
                }

                if ('\n' == c)
                {
                    ++size;
                }
            }
            io.setPos(pos);
            //DJV_DEBUG_PRINT("size = " << size);

            // Information.
            info.size = glm::ivec2(size, 1);
            int bitDepth = 0;
            switch (type)
            {
            case TYPE_AUTO: bitDepth = _bitDepth(io); break;
            case TYPE_U8:   bitDepth = 8;             break;
            case TYPE_U10:  bitDepth = 10;            break;
            case TYPE_U16:  bitDepth = 16;            break;
            default: break;
            }
            //DJV_DEBUG_PRINT("bit depth = " << bitDepth);
            if (!Pixel::pixel(channels, bitDepth, Pixel::INTEGER, info.pixel))
            {
                throw djvError(
                    LUT::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
        }

        void LUT::kodakOpen(djvFileIO &, const PixelDataInfo &)
            throw (djvError)
        {}

        void LUT::infernoLoad(djvFileIO & io, Image & out)
            throw (djvError)
        {
            //DJV_DEBUG("djvLUT::infernoLoad");
            QVector<Color> color(out.w());
            for (int x = 0; x < out.w(); ++x)
            {
                color[x].setPixel(out.pixel());
            }
            for (int c = 0; c < Pixel::channels(out.pixel()); ++c)
                for (int x = 0; x < out.w(); ++x)
                {
                    char tmp[djvStringUtil::cStringLength] = "";
                    djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
                    const int v = QString(tmp).toInt();
                    switch (Pixel::type(out.pixel()))
                    {
                    case Pixel::U8:  color[x].setU8(v, c); break;
                    case Pixel::U10: color[x].setU10(v, c); break;
                    case Pixel::U16: color[x].setU16(v, c); break;
                    default: break;
                    }
                }
            for (int x = 0; x < out.w(); ++x)
            {
                //DJV_DEBUG_PRINT(x << " = " << color[x]);
                memcpy(
                    out.data(x, 0),
                    color[x].data(),
                    Pixel::byteCount(out.pixel()));
            }
        }

        void LUT::kodakLoad(djvFileIO & io, Image & out) throw (djvError)
        {
            //DJV_DEBUG("LUT::kodakLoad");
            for (int x = 0; x < out.w(); ++x)
            {
                Color color(out.pixel());
                for (int c = 0; c < Pixel::channels(out.pixel()); ++c)
                {
                    char tmp[djvStringUtil::cStringLength] = "";
                    djvFileIOUtil::word(io, tmp, djvStringUtil::cStringLength);
                    const int v = QString(tmp).toInt();
                    switch (Pixel::type(out.pixel()))
                    {
                    case Pixel::U8:  color.setU8(v, c); break;
                    case Pixel::U10: color.setU10(v, c); break;
                    case Pixel::U16: color.setU16(v, c); break;
                    default: break;
                    }
                }
                //DJV_DEBUG_PRINT(x << " = " << color);
                memcpy(
                    out.data(x, 0),
                    color.data(),
                    Pixel::byteCount(out.pixel()));
            }
        }

        void LUT::infernoSave(djvFileIO & io, const PixelData * out)
            throw (djvError)
        {
            QVector<Color> color(out->w());
            for (int x = 0; x < out->w(); ++x)
            {
                color[x].setPixel(out->pixel());
                memcpy(
                    color[x].data(),
                    out->data(x, 0),
                    Pixel::byteCount(out->pixel()));
            }
            for (int c = 0; c < Pixel::channels(out->pixel()); ++c)
            {
                for (int x = 0; x < out->w(); ++x)
                {
                    int v = 0;
                    switch (Pixel::type(out->pixel()))
                    {
                    case Pixel::U8:  v = color[x].u8(c);  break;
                    case Pixel::U10: v = color[x].u10(c); break;
                    case Pixel::U16: v = color[x].u16(c); break;
                    default: break;
                    }
                    char tmp[djvStringUtil::cStringLength] = "";
                    const int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "%9d\n", v);
                    io.set(tmp, size);
                }
            }
        }

        void LUT::kodakSave(djvFileIO & io, const PixelData * out)
            throw (djvError)
        {
            for (int x = 0; x < out->w(); ++x)
            {
                Color color(out->pixel());
                memcpy(
                    color.data(),
                    out->data(x, 0),
                    Pixel::byteCount(out->pixel()));
                for (int c = 0; c < Pixel::channels(out->pixel()); ++c)
                {
                    int v = 0;
                    switch (Pixel::type(out->pixel()))
                    {
                    case Pixel::U8:  v = color.u8(c); break;
                    case Pixel::U10: v = color.u10(c); break;
                    case Pixel::U16: v = color.u16(c); break;
                    default: break;
                    }
                    char tmp[djvStringUtil::cStringLength] = "";
                    int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "%6d", v);
                    io.set(tmp, size);
                }
                io.set8('\n');
            }
        }

        const QStringList & LUT::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                "Type";
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace Graphics
} // namespace djv

_DJV_STRING_OPERATOR_LABEL(Graphics::LUT::FORMAT, Graphics::LUT::formatLabels())
_DJV_STRING_OPERATOR_LABEL(Graphics::LUT::TYPE, Graphics::LUT::typeLabels())
