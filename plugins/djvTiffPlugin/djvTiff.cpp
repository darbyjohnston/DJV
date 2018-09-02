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

#include <djvTiff.h>

#include <djvAssert.h>
#include <djvError.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvTiff::Options
//------------------------------------------------------------------------------

djvTiff::Options::Options() :
    compression(djvTiff::_COMPRESSION_NONE)
{}

//------------------------------------------------------------------------------
// djvTiff
//------------------------------------------------------------------------------

const QString djvTiff::staticName = "TIFF";

const QStringList & djvTiff::compressionLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvTiff", "None") <<
        qApp->translate("djvTiff", "RLE") <<
        qApp->translate("djvTiff", "LZW");
    DJV_ASSERT(data.count() == COMPRESSION_COUNT);
    return data;
}

void djvTiff::paletteLoad(
    quint8 *  in,
    int       size,
    int       bytes,
    quint16 * red,
    quint16 * green,
    quint16 * blue)
{
    switch (bytes)
    {
        case 1:
        {
            const quint8 * inP = in + size - 1;
            quint8 * outP = in + (size - 1) * 3;
            for (int x = 0; x < size; ++x, outP -= 3)
            {
                const quint8 index = *inP--;
                outP[0] = static_cast<quint8>(red[index]);
                outP[1] = static_cast<quint8>(green[index]);
                outP[2] = static_cast<quint8>(blue[index]);
            }
        }
        break;
        case 2:
        {
            const quint16 * inP =
                reinterpret_cast<const quint16 *>(in) + size - 1;
            quint16 * outP =
                reinterpret_cast<quint16 *>(in) + (size - 1) * 3;
            for (int x = 0; x < size; ++x, outP -= 3)
            {
                const quint16 index = *inP--;
                outP[0] = red[index];
                outP[1] = green[index];
                outP[2] = blue[index];
            }
        }
        break;
    }
}

const QStringList & djvTiff::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvTiff", "Compression");
    DJV_ASSERT(data.count() == OPTIONS_COUNT);
    return data;
}

_DJV_STRING_OPERATOR_LABEL(djvTiff::COMPRESSION, djvTiff::compressionLabels())
