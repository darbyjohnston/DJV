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

//! \file djvPicPlugin.cpp

#include <djvPicPlugin.h>

#include <djvPicLoad.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvMemory.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvPicPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvPicPlugin
//------------------------------------------------------------------------------

const QString djvPicPlugin::staticName = "PIC";

const QStringList & djvPicPlugin::compressionLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "RLE";

    DJV_ASSERT(data.count() == COMPRESSION_COUNT);

    return data;
}

const quint8 * djvPicPlugin::readRle(
    const quint8 * in,
    const quint8 * end,
    quint8 *       out,
    int            size,
    int            channels,
    int            stride,
    bool           endian)
{
    //DJV_DEBUG("readRle");
    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("channels = " << channels);
    //DJV_DEBUG_PRINT("stride = " << stride);

    const quint8 * const outEnd = out + size * stride;

    while (in < end && out < outEnd)
    {
        // Get RLE information.

        quint16 count = *in++;

        //DJV_DEBUG_PRINT("count = " << count);
        
        if (count >= 128)
        {
            if (128 == count)
            {
                if (endian)
                {
                    djvMemory::convertEndian(in, &count, 1, 2);
                }
                else
                {
                    djvMemory::copy(in, &count, 2);
                }

                in += 2;
            }
            else
            {
                count -= 127;
            }

            //DJV_DEBUG_PRINT("repeat = " << count);

            const quint8 * p = in;
            
            in += channels;

            if (in > end)
            {
                break;
            }

            for (quint16 i = 0; i < count; ++i, out += stride)
            {
                for (int j = 0; j < channels; ++j)
                {
                    out[j] = p[j];
                }
            }
        }
        else
        {
            ++count;
            
            //DJV_DEBUG_PRINT("raw = " << count);

            const quint8 * p = in;
            
            in += count * channels;

            if (in > end)
            {
                break;
            }

            for (quint16 i = 0; i < count; ++i, p += channels, out += stride)
            {
                for (int j = 0; j < channels; ++j)
                {
                    out[j] = p[j];
                }
            }
        }
    }

    return in > end ? 0 : in;
}

djvPlugin * djvPicPlugin::copyPlugin() const
{
    return new djvPicPlugin;
}

QString djvPicPlugin::pluginName() const
{
    return staticName;
}

QStringList djvPicPlugin::extensions() const
{
    return QStringList() << ".pic";
}
    
djvImageLoad * djvPicPlugin::createLoad() const
{
    return new djvPicLoad;
}

