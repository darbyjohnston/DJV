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

//! \file djvPpm.cpp

#include <djvPpm.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvMath.h>

#include <QApplication>

//------------------------------------------------------------------------------
// djvPpm::Options
//------------------------------------------------------------------------------

djvPpm::Options::Options() :
    type(TYPE_AUTO),
    data(DATA_BINARY)
{}

//------------------------------------------------------------------------------
// djvPpm
//------------------------------------------------------------------------------

const QString djvPpm::staticName = "PPM";

const QStringList & djvPpm::typeLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvPpm", "Auto") <<
        qApp->translate("djvPpm", "U1");

    DJV_ASSERT(data.count() == TYPE_COUNT);

    return data;
}

const QStringList & djvPpm::dataLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvPpm", "ASCII") <<
        qApp->translate("djvPpm", "Binary");

    DJV_ASSERT(data.count() == DATA_COUNT);

    return data;
}

quint64 djvPpm::scanlineByteCount(
    int  width,
    int  channels,
    int  bitDepth,
    DATA data)
{
    //DJV_DEBUG("djvPpm::scanlineByteCount");
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
                
                    out = djvMath::ceil(width / 8.0);
                    
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

void djvPpm::asciiLoad(djvFileIo & io, void * out, int size, int bitDepth)
    throw (djvError)
{
    //DJV_DEBUG("djvPpm::asciiLoad");

    char tmp[djvStringUtil::cStringLength] = "";
    
    int i = 0;

    switch (bitDepth)
    {
        case 1:
        {
            quint8 * outP = reinterpret_cast<quint8 *>(out);

            for (; i < size; ++i)
            {
                djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
                outP[i] = QString(tmp).toInt() ? 0 : 255;
            }
        }
        break;

#define _LOAD(TYPE) \
  \
    TYPE * outP = reinterpret_cast<TYPE *>(out); \
    for (; i < size; ++i) \
    { \
        djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength); \
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

quint64 djvPpm::asciiSave(
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
                outP[0] = '0' + (! inP[i]);
                outP[1] = ' ';
                outP += 2;
            }
        }
            break;

#define _SAVE(TYPE) \
    \
    const TYPE * inP = reinterpret_cast<const TYPE *>(in); \
    \
    for (int i = 0; i < size; ++i) \
    { \
        QString s = QString::number(inP[i]); \
        \
        const char * c = s.toLatin1().data(); \
        \
        for (int j = 0; j < s.count(); ++j) \
            *outP++ = c[j]; \
        \
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

const QStringList & djvPpm::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvPpm", "Type") <<
        qApp->translate("djvPpm", "Data");

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvPpm::TYPE, djvPpm::typeLabels())
_DJV_STRING_OPERATOR_LABEL(djvPpm::DATA, djvPpm::dataLabels())
