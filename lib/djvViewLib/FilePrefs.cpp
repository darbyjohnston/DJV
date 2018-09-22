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

#include <djvViewLib/FilePrefs.h>

#include <djvViewLib/FileCache.h>

#include <djvUI/Prefs.h>

#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>

namespace djv
{
    namespace ViewLib
    {
        FilePrefs::FilePrefs(Context * context, QObject * parent) :
            AbstractPrefs(context, parent),
            _autoSequence(autoSequenceDefault()),
            _proxy(proxyDefault()),
            _u8Conversion(u8ConversionDefault()),
            _cache(cacheDefault()),
            _cacheSize(cacheSizeDefault()),
            _preload(preloadDefault()),
            _displayCache(displayCacheDefault())
        {
            UI::Prefs prefs("djv::ViewLib::FilePrefs");
            prefs.get("recent", _recent);
            prefs.get("autoSequence", _autoSequence);
            prefs.get("proxy", _proxy);
            prefs.get("u8Conversion", _u8Conversion);
            prefs.get("cache", _cache);
            prefs.get("cacheSize", _cacheSize);
            prefs.get("preload", _preload);
            prefs.get("displayCache", _displayCache);
            if (_recent.count() > Core::FileInfoUtil::recentMax)
                _recent = _recent.mid(0, Core::FileInfoUtil::recentMax);
        }

        FilePrefs::~FilePrefs()
        {
            UI::Prefs prefs("djv::ViewLib::FilePrefs");
            prefs.set("recent", _recent);
            prefs.set("autoSequence", _autoSequence);
            prefs.set("proxy", _proxy);
            prefs.set("u8Conversion", _u8Conversion);
            prefs.set("cache", _cache);
            prefs.set("cacheSize", _cacheSize);
            prefs.set("preload", _preload);
            prefs.set("displayCache", _displayCache);
        }

        void FilePrefs::addRecent(const Core::FileInfo & in)
        {
            //DJV_DEBUG("FilePrefs::addRecent");
            //DJV_DEBUG_PRINT("in = " << in);
            Core::FileInfoUtil::recent(in, _recent);
            Q_EMIT recentChanged(_recent);
            Q_EMIT prefChanged();
        }

        const Core::FileInfoList & FilePrefs::recentFiles() const
        {
            return _recent;
        }

        bool FilePrefs::autoSequenceDefault()
        {
            return true;
        }

        bool FilePrefs::hasAutoSequence() const
        {
            return _autoSequence;
        }

        Graphics::PixelDataInfo::PROXY FilePrefs::proxyDefault()
        {
            return static_cast<Graphics::PixelDataInfo::PROXY>(0);
        }

        Graphics::PixelDataInfo::PROXY FilePrefs::proxy() const
        {
            return _proxy;
        }

        bool FilePrefs::u8ConversionDefault()
        {
            return false;
        }

        bool FilePrefs::hasU8Conversion() const
        {
            return _u8Conversion;
        }

        bool FilePrefs::cacheDefault()
        {
            return true;
        }

        bool FilePrefs::hasCache() const
        {
            return _cache;
        }

        float FilePrefs::cacheSizeDefault()
        {
            return FileCache::sizeDefaults()[0];
        }

        float FilePrefs::cacheSize() const
        {
            return _cacheSize;
        }

        bool FilePrefs::preloadDefault()
        {
            return false;
        }

        bool FilePrefs::hasPreload() const
        {
            return _preload;
        }

        bool FilePrefs::displayCacheDefault()
        {
            return true;
        }

        bool FilePrefs::hasDisplayCache() const
        {
            return _displayCache;
        }

        void FilePrefs::setAutoSequence(bool autoSequence)
        {
            if (autoSequence == _autoSequence)
                return;
            _autoSequence = autoSequence;
            Q_EMIT autoSequenceChanged(_autoSequence);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setProxy(Graphics::PixelDataInfo::PROXY proxy)
        {
            if (proxy == _proxy)
                return;
            _proxy = proxy;
            Q_EMIT proxyChanged(_proxy);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setU8Conversion(bool conversion)
        {
            if (conversion == _u8Conversion)
                return;
            _u8Conversion = conversion;
            Q_EMIT u8ConversionChanged(_u8Conversion);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setCache(bool cache)
        {
            if (cache == _cache)
                return;
            _cache = cache;
            Q_EMIT cacheChanged(_cache);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setCacheSize(float size)
        {
            if (size == _cacheSize)
                return;
            //DJV_DEBUG("FilePrefs::cacheSize");
            //DJV_DEBUG_PRINT("size = " << size);
            _cacheSize = size;
            Q_EMIT cacheSizeChanged(_cacheSize);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setPreload(bool preload)
        {
            if (preload == _preload)
                return;
            _preload = preload;
            Q_EMIT preloadChanged(_preload);
            Q_EMIT prefChanged();
        }

        void FilePrefs::setDisplayCache(bool display)
        {
            if (display == _displayCache)
                return;
            _displayCache = display;
            Q_EMIT displayCacheChanged(_displayCache);
            Q_EMIT prefChanged();
        }

    } // namespace ViewLib
} // namespace djv
