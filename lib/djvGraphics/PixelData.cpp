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

#include <djvGraphics/PixelData.h>

#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/FileIO.h>
#include <djvCore/StringUtil.h>

#include <QCoreApplication>

using namespace djv;

namespace djv
{
    namespace Graphics
    {
        PixelDataInfo::Mirror::Mirror()
        {}

        PixelDataInfo::Mirror::Mirror(bool x, bool y) :
            x(x),
            y(y)
        {}

        void PixelDataInfo::init()
        {
            static const QString defaultLayerName = qApp->translate("djv::Graphisc::PixelDataInfo", "Default");
            layerName = defaultLayerName;
        }

        PixelDataInfo::PixelDataInfo()
        {
            init();
        }

        PixelDataInfo::PixelDataInfo(
            const glm::ivec2 & size,
            Pixel::PIXEL       pixel)
        {
            init();
            this->size = size;
            this->pixel = pixel;
        }

        PixelDataInfo::PixelDataInfo(
            int          width,
            int          height,
            Pixel::PIXEL pixel)
        {
            init();
            this->size = glm::ivec2(width, height);
            this->pixel = pixel;
        }

        PixelDataInfo::PixelDataInfo(
            const QString &    fileName,
            const glm::ivec2 & size,
            Pixel::PIXEL       pixel)
        {
            init();
            this->fileName = fileName;
            this->size = size;
            this->pixel = pixel;
        }

        PixelDataInfo::PixelDataInfo(
            const QString & fileName,
            int             width,
            int             height,
            Pixel::PIXEL    pixel)
        {
            init();
            this->fileName = fileName;
            this->size = glm::ivec2(width, height);
            this->pixel = pixel;
        }

        const QStringList & PixelDataInfo::proxyLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::PixelDataInfo", "None") <<
                qApp->translate("djv::Graphics::PixelDataInfo", "1/2") <<
                qApp->translate("djv::Graphics::PixelDataInfo", "1/4") <<
                qApp->translate("djv::Graphics::PixelDataInfo", "1/8");
            DJV_ASSERT(data.count() == PixelDataInfo::PROXY_COUNT);
            return data;
        }

        PixelData::PixelData()
        {
            //DJV_DEBUG("PixelData::PixelData");
        }

        PixelData::PixelData(const PixelData & in)
        {
            //DJV_DEBUG("PixelData::PixelData");
            copy(in);
        }

        PixelData::PixelData(
            const PixelDataInfo & in,
            const quint8 *        p,
            djvFileIO *           fileIo)
        {
            //DJV_DEBUG("PixelData::PixelData");
            set(in, p, fileIo);
        }

        PixelData::~PixelData()
        {
            delete _fileIo;
        }

        void PixelData::zero()
        {
            //DJV_DEBUG("PixelData::zero");
            memset(_data.data(), 0, _dataByteCount);
        }

        void PixelData::close()
        {
            //DJV_DEBUG("PixelData::close");
            if (_fileIo)
            {
                delete _fileIo;
                _info = PixelDataInfo();
                _channels = 0;
                _data.resize(0);
                _p = nullptr;
                _pixelByteCount = 0;
                _scanlineByteCount = 0;
                _dataByteCount = 0;
                _fileIo = nullptr;
            }
        }

        PixelData & PixelData::operator = (const PixelData & in)
        {
            if (&in != this)
            {
                copy(in);
            }
            return *this;
        }

        void PixelData::detach()
        {
            if (_fileIo)
            {
                _data.resize(_dataByteCount);
                memcpy(_data.data(), _p, _dataByteCount);
                _p = _data.data();
                delete _fileIo;
                _fileIo = 0;
            }
        }

        void PixelData::set(
            const PixelDataInfo & in,
            const quint8 *        p,
            djvFileIO *           fileIo)
        {
            //DJV_DEBUG("PixelData::set");
            //DJV_DEBUG_PRINT("in = " << in);

            delete _fileIo;
            _fileIo = 0;

            _info = in;

            _channels = Pixel::channels(_info.pixel);
            _pixelByteCount = Pixel::byteCount(_info.pixel);
            _scanlineByteCount = PixelDataUtil::scanlineByteCount(_info);
            _dataByteCount = PixelDataUtil::dataByteCount(_info);
            //DJV_DEBUG_PRINT("channels = " << _channels);
            //DJV_DEBUG_PRINT("bytes pixel = " << _pixelByteCount);
            //DJV_DEBUG_PRINT("bytes scanline = " << _scanlineByteCount);
            //DJV_DEBUG_PRINT("bytes data = " << _dataByteCount);

            if (p)
            {
                if (fileIo)
                {
                    _data.resize(0);
                    _p = p;
                    _fileIo = fileIo;
                }
                else
                {
                    _data.resize(_dataByteCount);
                    _p = _data.data();
                    memcpy(_data.data(), p, _dataByteCount);
                }
            }
            else
            {
                _data.resize(_dataByteCount);
                _p = _data.data();
            }
        }

        void PixelData::copy(const PixelData & in)
        {
            set(in._info);
            memcpy(_data.data(), in._p, _dataByteCount);
        }

        bool operator == (const PixelDataInfo::Mirror & a, const PixelDataInfo::Mirror & b)
        {
            return
                a.x == b.x &&
                a.y == b.y;
        }

        bool operator == (const PixelDataInfo & a, const PixelDataInfo & b)
        {
            return
                a.size == b.size   &&
                a.proxy == b.proxy  &&
                a.pixel == b.pixel  &&
                a.bgr == b.bgr    &&
                a.mirror == b.mirror &&
                a.align == b.align  &&
                a.endian == b.endian;
        }

        bool operator == (const PixelData & a, const PixelData & b)
        {
            return
                a.info() == b.info() &&
                a.dataByteCount() == b.dataByteCount() &&
                memcmp(a.data(), b.data(), a.dataByteCount()) == 0;
        }

        bool operator != (const PixelDataInfo::Mirror & a, const PixelDataInfo::Mirror & b)
        {
            return !(a == b);
        }

        bool operator != (const PixelDataInfo & a, const PixelDataInfo & b)
        {
            return !(a == b);
        }

        bool operator != (const PixelData & a, const PixelData & b)
        {
            return !(a == b);
        }

    } // namespace Graphics
} // namespace djv

_DJV_STRING_OPERATOR_LABEL(
    Graphics::PixelDataInfo::PROXY,
    Graphics::PixelDataInfo::proxyLabels())

QStringList & operator >> (QStringList & in, Graphics::PixelDataInfo::Mirror & out) throw (QString)
{
    in >> out.x;
    in >> out.y;    
    return in;
}

QStringList & operator << (QStringList & out, const Graphics::PixelDataInfo::Mirror & in)
{
    out << in.x;
    out << in.y;
    return out;
}

djvDebug & operator << (djvDebug & debug, const Graphics::PixelDataInfo::PROXY & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const Graphics::PixelDataInfo::Mirror & in)
{
    return debug << in.x << " " << in.y;
}

djvDebug & operator << (djvDebug & debug, const Graphics::PixelDataInfo & in)
{
    return debug <<
        //"file name = " << in.fileName << ", " <<
        //"layer name = " << in.layerName << ", " <<
        "size = " << in.size << ", " <<
        "proxy = " << in.proxy << ", " <<
        "pixel = " << in.pixel << ", " <<
        "bgr = " << in.bgr << ", " <<
        "mirror = " << in.mirror << ", " <<
        "align = " << in.align << ", " <<
        "endian = " << in.endian;
}

djvDebug & operator << (djvDebug & debug, const Graphics::PixelData & in)
{
    return debug << in.info();
}

