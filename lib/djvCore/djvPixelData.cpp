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

//! \file djvPixelData.cpp

#include <djvPixelData.h>

#include <djvPixelDataUtil.h>

#include <djvAssert.h>
#include <djvDebug.h>
#include <djvFileIo.h>
#include <djvStringUtil.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvPixelDataInfo::Mirror
//------------------------------------------------------------------------------

djvPixelDataInfo::Mirror::Mirror(bool x, bool y) :
    x(x),
    y(y)
{}

//------------------------------------------------------------------------------
// djvPixelDataInfo
//------------------------------------------------------------------------------

void djvPixelDataInfo::init()
{
    static const QString defaultLayerName =
        qApp->translate("djvPixelDataInfo", "Default");
    
    layerName = defaultLayerName;
    proxy     = PROXY_NONE;
    pixel     = static_cast<djvPixel::PIXEL>(0);
    bgr       = false;
    align     = 1;
    endian    = djvMemory::endian();
}

djvPixelDataInfo::djvPixelDataInfo()
{
    init();
}

djvPixelDataInfo::djvPixelDataInfo(
    const djvVector2i & size,
    djvPixel::PIXEL     pixel)
{
    init();

    this->size  = size;
    this->pixel = pixel;
}

djvPixelDataInfo::djvPixelDataInfo(
    int             width,
    int             height,
    djvPixel::PIXEL pixel)
{
    init();

    this->size   = djvVector2i(width, height);
    this->pixel  = pixel;
}

djvPixelDataInfo::djvPixelDataInfo(
    const QString &     fileName,
    const djvVector2i & size,
    djvPixel::PIXEL     pixel)
{
    init();

    this->fileName = fileName;
    this->size     = size;
    this->pixel    = pixel;
}

djvPixelDataInfo::djvPixelDataInfo(
    const QString & fileName,
    int             width,
    int             height,
    djvPixel::PIXEL pixel)
{
    init();

    this->fileName = fileName;
    this->size     = djvVector2i(width, height);
    this->pixel    = pixel;
}

const QStringList & djvPixelDataInfo::proxyLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvPixelDataInfo", "None") <<
        qApp->translate("djvPixelDataInfo", "1/2") <<
        qApp->translate("djvPixelDataInfo", "1/4") <<
        qApp->translate("djvPixelDataInfo", "1/8");

    DJV_ASSERT(data.count() == djvPixelDataInfo::PROXY_COUNT);

    return data;
}

//------------------------------------------------------------------------------
// djvPixelData
//------------------------------------------------------------------------------

djvPixelData::djvPixelData()
{
    //DJV_DEBUG("djvPixelData::djvPixelData");

    init();
}

djvPixelData::djvPixelData(const djvPixelData & in)
{
    //DJV_DEBUG("djvPixelData::djvPixelData");

    init();

    copy(in);
}

djvPixelData::djvPixelData(
    const djvPixelDataInfo & in,
    const quint8 *           p,
    djvFileIo *              fileIo)
{
    //DJV_DEBUG("djvPixelData::djvPixelData");

    init();

    set(in, p, fileIo);
}

djvPixelData::~djvPixelData()
{
	delete _fileIo;
}

void djvPixelData::zero()
{
    //DJV_DEBUG("djvPixelData::zero");

    _data.zero();
}

void djvPixelData::close()
{
    //DJV_DEBUG("djvPixelData::close");
    
    if (_fileIo)
    {
        delete _fileIo;
        
        init();
    }
}

djvPixelData & djvPixelData::operator = (const djvPixelData & in)
{
    if (&in != this)
    {
        copy(in);
    }

    return *this;
}

void djvPixelData::init()
{
    _channels          = 0;
    _p                 = 0;
    _pixelByteCount    = 0;
    _scanlineByteCount = 0;
    _dataByteCount     = 0;
    _fileIo            = 0;
}

void djvPixelData::detach()
{
    if (_fileIo)
    {
        _data.setSize(_dataByteCount);
        
        djvMemory::copy(_p, _data(), _dataByteCount);
        
        _p = _data();

        delete _fileIo;

        _fileIo = 0;
    }
}

void djvPixelData::set(
    const djvPixelDataInfo & in,
    const quint8 *           p,
    djvFileIo *              fileIo)
{
    //DJV_DEBUG("djvPixelData::set");
    //DJV_DEBUG_PRINT("in = " << in);
    
	delete _fileIo;
	
    _fileIo = 0;

    _info = in;

    _channels          = djvPixel::channels(_info.pixel);
    _pixelByteCount    = djvPixel::byteCount(_info.pixel);
    _scanlineByteCount = djvPixelDataUtil::scanlineByteCount(_info);
    _dataByteCount     = djvPixelDataUtil::dataByteCount(_info);

    //DJV_DEBUG_PRINT("channels = " << _channels);
    //DJV_DEBUG_PRINT("bytes pixel = " << _pixelByteCount);
    //DJV_DEBUG_PRINT("bytes scanline = " << _scanlineByteCount);
    //DJV_DEBUG_PRINT("bytes data = " << _dataByteCount);

    if (p)
    {
        if (fileIo)
        {
            _data.setSize(0);
            
            _p = p;
            
            _fileIo = fileIo;
        }
        else
        {
            _data.setSize(_dataByteCount);
            
            _p = _data.data();
    
            djvMemory::copy(p, _data(), _dataByteCount);
        }
    }
    else
    {
        _data.setSize(_dataByteCount);

        _p = _data.data();
    }
}

void djvPixelData::copy(const djvPixelData & in)
{
    set(in._info);
    
    djvMemory::copy(in._p, _data(), _dataByteCount);
}

//------------------------------------------------------------------------------

bool operator == (const djvPixelDataInfo::Mirror & a,
    const djvPixelDataInfo::Mirror & b)
{
    return
        a.x == b.x &&
        a.y == b.y;
}

bool operator == (const djvPixelDataInfo & a, const djvPixelDataInfo & b)
{
    return
        a.size   == b.size   &&
        a.proxy  == b.proxy  &&
        a.pixel  == b.pixel  &&
        a.bgr    == b.bgr    &&
        a.mirror == b.mirror &&
        a.align  == b.align  &&
        a.endian == b.endian;
}

bool operator == (const djvPixelData & a, const djvPixelData & b)
{
    return
        a.info()          == b.info()          &&
        a.dataByteCount() == b.dataByteCount() &&
        djvMemory::compare(a.data(), b.data(), a.dataByteCount()) == 0;
}

bool operator != (const djvPixelDataInfo::Mirror & a,
    const djvPixelDataInfo::Mirror & b)
{
    return ! (a == b);
}

bool operator != (const djvPixelDataInfo & a, const djvPixelDataInfo & b)
{
    return ! (a == b);
}

bool operator != (const djvPixelData & a, const djvPixelData & b)
{
    return ! (a == b);
}

_DJV_STRING_OPERATOR_LABEL(
    djvPixelDataInfo::PROXY,
    djvPixelDataInfo::proxyLabels())

QStringList & operator >> (QStringList & in, djvPixelDataInfo::Mirror & out) throw (QString)
{
    in >> out.x;
    in >> out.y;
    
    return in;
}

QStringList & operator << (QStringList & out, const djvPixelDataInfo::Mirror & in)
{
    out << in.x;
    out << in.y;
    
    return out;
}

djvDebug & operator << (djvDebug & debug, const djvPixelDataInfo::PROXY & in)
{
    return debug << djvStringUtil::label(in);
}

djvDebug & operator << (djvDebug & debug, const djvPixelDataInfo::Mirror & in)
{
    return debug << in.x << " " << in.y;
}

djvDebug & operator << (djvDebug & debug, const djvPixelDataInfo & in)
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

djvDebug & operator << (djvDebug & debug, const djvPixelData & in)
{
    return debug << in.info();
}

