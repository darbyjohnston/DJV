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

//! \file djvViewFilePrefs.cpp

#include <djvViewFilePrefs.h>

#include <djvViewApplication.h>
#include <djvViewFileCache.h>

#include <djvPrefs.h>

#include <djvFileInfoUtil.h>
#include <djvListUtil.h>

//------------------------------------------------------------------------------
// djvViewFilePrefs
//------------------------------------------------------------------------------

djvViewFilePrefs::djvViewFilePrefs(QObject * parent) :
    djvViewAbstractPrefs(parent),
    _autoSequence      (autoSequenceDefault()),
    _proxy             (proxyDefault()),
    _u8Conversion      (u8ConversionDefault()),
    _cacheEnabled      (cacheEnabledDefault()),
    _cacheSize         (cacheSizeDefault()),
    _cacheDisplay      (cacheDisplayDefault())
{
    djvPrefs prefs("djvViewFilePrefs");

    prefs.get("recent", _recent);
    prefs.get("autoSequence", _autoSequence);
    prefs.get("proxy", _proxy);
    prefs.get("u8Conversion", _u8Conversion);
    prefs.get("cacheEnabled", _cacheEnabled);
    prefs.get("cacheSize", _cacheSize);
    prefs.get("cacheDisplay", _cacheDisplay);

    if (_recent.count() > djvFileInfoUtil::recentMax)
        _recent = _recent.mid(0, djvFileInfoUtil::recentMax);
}

djvViewFilePrefs::~djvViewFilePrefs()
{
    djvPrefs prefs("djvViewFilePrefs");

    prefs.set("recent", _recent);
    prefs.set("autoSequence", _autoSequence);
    prefs.set("proxy", _proxy);
    prefs.set("u8Conversion", _u8Conversion);
    prefs.set("cacheEnabled", _cacheEnabled);
    prefs.set("cacheSize", _cacheSize);
    prefs.set("cacheDisplay", _cacheDisplay);
}

void djvViewFilePrefs::addRecent(const djvFileInfo & in)
{
    //DJV_DEBUG("djvViewFilePrefs::addRecent");
    //DJV_DEBUG_PRINT("in = " << in);

    djvFileInfoUtil::recent(in, _recent);

    Q_EMIT recentChanged(_recent);
    Q_EMIT prefChanged();
}

const djvFileInfoList & djvViewFilePrefs::recentFiles() const
{
    return _recent;
}

bool djvViewFilePrefs::autoSequenceDefault()
{
    return true;
}

bool djvViewFilePrefs::hasAutoSequence() const
{
    return _autoSequence;
}

djvPixelDataInfo::PROXY djvViewFilePrefs::proxyDefault()
{
    return static_cast<djvPixelDataInfo::PROXY>(0);
}

djvPixelDataInfo::PROXY djvViewFilePrefs::proxy() const
{
    return _proxy;
}

bool djvViewFilePrefs::u8ConversionDefault()
{
    return false;
}

bool djvViewFilePrefs::hasU8Conversion() const
{
    return _u8Conversion;
}

bool djvViewFilePrefs::cacheEnabledDefault()
{
    return true;
}

bool djvViewFilePrefs::isCacheEnabled() const
{
    return _cacheEnabled;
}

double djvViewFilePrefs::cacheSizeDefault()
{
    return djvViewFileCache::sizeDefaults()[0];
}

double djvViewFilePrefs::cacheSize() const
{
    return _cacheSize;
}

bool djvViewFilePrefs::cacheDisplayDefault()
{
    return true;
}

bool djvViewFilePrefs::hasCacheDisplay() const
{
    return _cacheDisplay;
}

djvViewFilePrefs * djvViewFilePrefs::global()
{
    static djvViewFilePrefs * prefs = 0;

    if (! prefs)
    {
        prefs = new djvViewFilePrefs(DJV_VIEW_APP);
    }

    return prefs;
}

void djvViewFilePrefs::setAutoSequence(bool autoSequence)
{
    if (autoSequence == _autoSequence)
        return;

    _autoSequence = autoSequence;

    Q_EMIT autoSequenceChanged(_autoSequence);
    Q_EMIT prefChanged();
}

void djvViewFilePrefs::setProxy(djvPixelDataInfo::PROXY proxy)
{
    if (proxy == _proxy)
        return;

    _proxy = proxy;

    Q_EMIT proxyChanged(_proxy);
    Q_EMIT prefChanged();
}

void djvViewFilePrefs::setU8Conversion(bool conversion)
{
    if (conversion == _u8Conversion)
        return;

    _u8Conversion = conversion;

    Q_EMIT u8ConversionChanged(_u8Conversion);
    Q_EMIT prefChanged();
}

void djvViewFilePrefs::setCacheEnabled(bool cache)
{
    if (cache == _cacheEnabled)
        return;

    _cacheEnabled = cache;

    Q_EMIT cacheEnabledChanged(_cacheEnabled);
    Q_EMIT prefChanged();
}

void djvViewFilePrefs::setCacheSize(double size)
{
    if (size == _cacheSize)
        return;

    //DJV_DEBUG("djvViewFilePrefs::cacheSize");
    //DJV_DEBUG_PRINT("size = " << size);

    _cacheSize = size;

    Q_EMIT cacheSizeChanged(_cacheSize);
    Q_EMIT prefChanged();
}

void djvViewFilePrefs::setCacheDisplay(bool display)
{
    if (display == _cacheDisplay)
        return;

    _cacheDisplay = display;

    Q_EMIT cacheDisplayChanged(_cacheDisplay);
    Q_EMIT prefChanged();
}

