//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/PPM.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>
#include <djvCore/FileIOUtil.h>
#include <djvCore/Math.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        const QString PPM::staticName = "PPM";

        const QStringList & PPM::typeLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::PPM", "Auto") <<
                qApp->translate("djv::AV::PPM", "U1");
            DJV_ASSERT(data.count() == TYPE_COUNT);
            return data;
        }

        const QStringList & PPM::dataLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::PPM", "ASCII") <<
                qApp->translate("djv::AV::PPM", "Binary");
            DJV_ASSERT(data.count() == DATA_COUNT);
            return data;
        }

        quint64 PPM::scanlineByteCount(
            int  width,
            int  channels,
            int  bitDepth,
            DATA data)
        {
            //DJV_DEBUG("PPM::scanlineByteCount");
            //DJV_DEBUG_PRINT("width = " << width);
            //DJV_DEBUG_PRINT("channels = " << channels);
            //DJV_DEBUG_PRINT("bit depth = " << bitDepth);
            //DJV_DEBUG_PRINT("data = " << data);

            quint64 out = 0;
            switch (data)
            {
            case DATA_ASCII:
            {
                int chars = 0;
                switch (bitDepth)
                {
                case  1: chars = 1; break;
                case  8: chars = 3; break;
                case 16: chars = 5; break;
                default: break;
                }
                out = (chars + 1) * width * channels + 1;
            }
            break;
            case DATA_BINARY:
            {
                switch (bitDepth)
                {
                case 1:
                    out = Core::Math::ceil(width / 8.f);
                    break;
                case  8:
                case 16:
                    out = width * channels;
                    break;
                default: break;
                }
            }
            break;
            default: break;
            }
            //DJV_DEBUG_PRINT("out = " << static_cast<int>(out));
            return out;
        }

        void PPM::asciiLoad(Core::FileIO & io, void * out, int size, int bitDepth)
        {
            //DJV_DEBUG("PPM::asciiLoad");
            char tmp[Core::StringUtil::cStringLength] = "";
            int i = 0;
            switch (bitDepth)
            {
            case 1:
            {
                quint8 * outP = reinterpret_cast<quint8 *>(out);
                for (; i < size; ++i)
                {
                    Core::FileIOUtil::word(io, tmp, Core::StringUtil::cStringLength);
                    outP[i] = QString(tmp).toInt() ? 0 : 255;
                }
            }
            break;
#define _LOAD(TYPE) \
    TYPE * outP = reinterpret_cast<TYPE *>(out); \
    for (; i < size; ++i) \
    { \
        Core::FileIOUtil::word(io, tmp, Core::StringUtil::cStringLength); \
        outP[i] = QString(tmp).toInt(); \
    }
            case 8:
            {
                _LOAD(quint8)
            }
            break;
            case 16:
            {
                _LOAD(quint16)
            }
            break;
            default: break;
            }
        }

        quint64 PPM::asciiSave(
            const void * in,
            void *       out,
            int          size,
            int          bitDepth)
        {
            char * outP = reinterpret_cast<char *>(out);
            switch (bitDepth)
            {
            case 1:
            {
                const quint8 * inP = reinterpret_cast<const quint8 *>(in);
                for (int i = 0; i < size; ++i)
                {
                    outP[0] = '0' + (!inP[i]);
                    outP[1] = ' ';
                    outP += 2;
                }
            }
            break;
#define _SAVE(TYPE) \
    const TYPE * inP = reinterpret_cast<const TYPE *>(in); \
    for (int i = 0; i < size; ++i) \
    { \
        QString s = QString::number(inP[i]); \
        const char * c = s.toLatin1().data(); \
        for (int j = 0; j < s.count(); ++j) \
            *outP++ = c[j]; \
        *outP++ = ' '; \
    }
            case 8:
            {
                _SAVE(quint8)
            }
            break;
            case 16:
            {
                _SAVE(quint16)
            }
            break;
            default: break;
            }
            *outP++ = '\n';
            return outP - reinterpret_cast<char *>(out);
        }

        const QStringList & PPM::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::PPM", "Type") <<
                qApp->translate("djv::AV::PPM", "Data");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::PPM::TYPE, AV::PPM::typeLabels());
    _DJV_STRING_OPERATOR_LABEL(AV::PPM::DATA, AV::PPM::dataLabels());

} // namespace djv
