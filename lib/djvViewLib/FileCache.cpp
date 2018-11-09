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
//------------------------------------------------------------------------------s

#include <djvViewLib/FileCache.h>

#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvAV/Image.h>

#include <djvCore/Assert.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Memory.h>
#include <djvCore/Time.h>

#include <QPointer>

#include <algorithm>
#include <functional>

namespace djv
{
    namespace ViewLib
    {
        FileCacheKey::FileCacheKey() :
            timestamp(Core::Time::current())
        {}

        FileCacheKey::FileCacheKey(void * window, qint64 frame) :
            window(window),
            frame(frame),
            timestamp(Core::Time::current())
        {}

        bool FileCacheKey::operator < (const FileCacheKey & other) const
        {
            if (window != other.window)
            {
                return window < other.window;
            }
            return frame < other.frame;
        }

        struct FileCache::Private
        {
            Private(const QPointer<ViewContext> & context) :
                maxBytes(static_cast<quint64>(context->filePrefs()->cacheSizeGB() * Core::Memory::gigabyte)),
                context(context)
            {}

            std::map<FileCacheKey, std::shared_ptr<AV::Image> > items;
            quint64 maxBytes = 0;
            quint64 cacheBytes = 0;
            QPointer<ViewContext> context;
        };

        FileCache::FileCache(const QPointer<ViewContext> & context, QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {
            //DJV_DEBUG("FileCache::FileCache");
            connect(
                context->filePrefs(),
                SIGNAL(cacheEnabledChanged(bool)),
                SLOT(cacheEnabledCallback(bool)));
            connect(
                context->filePrefs(),
                SIGNAL(cacheSizeGBChanged(float)),
                SLOT(cacheSizeGBCallback(float)));
        }

        FileCache::~FileCache()
        {
            //DJV_DEBUG("FileCache::~FileCache");
            //debug();
        }

        bool FileCache::hasItem(const FileCacheKey & key)
        {
            return _p->items.find(key) != _p->items.end();
        }

        std::shared_ptr<AV::Image> FileCache::item(const FileCacheKey & key) const
        {
            return _p->items.find(key)->second;
        }

        void FileCache::addItem(const FileCacheKey & key, const std::shared_ptr<AV::Image> & item)
        {
            _p->items[key] = item;
            _p->cacheBytes += item->dataByteCount();
            if (_p->cacheBytes > _p->maxBytes)
            {
                purge();
            }
            Q_EMIT cacheChanged();
            debug();
        }

        void FileCache::clearItems(void * window)
        {
            auto i = _p->items.begin();
            while (i != _p->items.end())
            {
                if (window == i->first.window)
                {
                    _p->cacheBytes -= i->second->dataByteCount();
                    i = _p->items.erase(i);
                }
                else
                {
                    ++i;
                }
            }
            Q_EMIT cacheChanged();
            debug();
        }

        void FileCache::clear()
        {
            auto i = _p->items.begin();
            while (i != _p->items.end())
            {
                _p->cacheBytes -= i->second->dataByteCount();
                i = _p->items.erase(i);
            }
            Q_EMIT cacheChanged();
            debug();
        }

        void FileCache::removeItem(const FileCacheKey & key)
        {
            auto i = _p->items.find(key);
            if (i != _p->items.end())
            {
                _p->cacheBytes -= i->second->dataByteCount();
                _p->items.erase(i);
            }
        }

        std::vector<std::shared_ptr<AV::Image> > FileCache::items(void * window)
        {
            std::vector<std::shared_ptr<AV::Image> > out;
            for (auto i = _p->items.begin(); i != _p->items.end(); ++i)
            {
                if (window == i->first.window)
                {
                    out.push_back(i->second);
                }
            }
            return out;
        }

        namespace
        {
            bool compare(qint64 a, qint64 b)
            {
                return a < b;
            }

        } // namespace

        Core::FrameList FileCache::frames(void * window)
        {
            Core::FrameList frames;
            for (auto i = _p->items.begin(); i != _p->items.end(); ++i)
            {
                if (window == i->first.window)
                {
                    frames.push_back(i->first.frame);
                }
            }
            qSort(frames.begin(), frames.end(), compare);
            return frames;
        }

        float FileCache::maxSizeGB() const
        {
            return _p->maxBytes / static_cast<float>(Core::Memory::gigabyte);
        }

        quint64 FileCache::maxSizeBytes() const
        {
            return _p->maxBytes;
        }

        float FileCache::currentSizeGB(void * window) const
        {
            quint64 size = 0;
            for (auto i = _p->items.begin(); i != _p->items.end(); ++i)
            {
                if (window == i->first.window)
                {
                    size += i->second->dataByteCount();
                }
            }
            return size / static_cast<float>(Core::Memory::gigabyte);
        }

        float FileCache::currentSizeGB() const
        {
            return _p->cacheBytes / static_cast<float>(Core::Memory::gigabyte);
        }

        quint64 FileCache::currentSizeBytes() const
        {
            return _p->cacheBytes;
        }

        const QVector<float> & FileCache::sizeGBDefaults()
        {
            static const QVector<float> data = QVector<float>() <<
                1.f <<
                2.f <<
                3.f <<
                6.f <<
                12.f <<
                24.f <<
                48.f <<
                96.f;
            return data;
        }

        void FileCache::debug()
        {
            /*DJV_DEBUG("FileCache::debug");
            DJV_DEBUG_PRINT("items = " << _p->items.size());
            DJV_DEBUG_PRINT("max = " << maxSizeGB());
            DJV_DEBUG_PRINT("size = " << currentSizeGB());
            for (auto i = _p->items.begin(); i != _p->items.end(); ++i)
            {
                DJV_DEBUG_PRINT(
                    "item (count = " <<
                    i->second.use_count() <<
                    ") = " <<
                    reinterpret_cast<qint64>(i->first.window) <<
                    " " <<
                    i->first.frame);
            }*/
        }

        void FileCache::setMaxSizeGB(float size)
        {
            //DJV_DEBUG("FileCache::setMaxSizeGB");
            //DJV_DEBUG_PRINT("size = " << size);
            //debug();
            _p->maxBytes = static_cast<quint64>(size * Core::Memory::gigabyte);
            //if (_p->cacheBytes > _p->maxBytes)
            purge();
            //debug();
        }

        void FileCache::purge()
        {
            //DJV_DEBUG("FileCache::purge");
            debug();

            // Delete as many items as possible to bring the cache size below the maximum size.
            std::map<::time_t, FileCacheKey> sortedByTime;
            for (auto i : _p->items)
            {
                sortedByTime[i.first.timestamp] = i.first;
            }
            auto j = sortedByTime.begin();
            while (_p->cacheBytes > _p->maxBytes && j != sortedByTime.end())
            {
                auto k = _p->items.find(j->second);
                if (k != _p->items.end())
                {
                    _p->cacheBytes -= k->second->dataByteCount();
                    _p->items.erase(k);
                    j = sortedByTime.erase(j);
                }
            }

            Q_EMIT cacheChanged();
            debug();
        }

        void FileCache::cacheEnabledCallback(bool cache)
        {
            if (!cache)
            {
                clear();
            }
        }

        void FileCache::cacheSizeGBCallback(float size)
        {
            setMaxSizeGB(size);
        }

    } // namespace ViewLib
} // namespace djv
