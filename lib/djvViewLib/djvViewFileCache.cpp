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
//------------------------------------------------------------------------------s

#include <djvViewFileCache.h>

#include <djvViewContext.h>
#include <djvViewFilePrefs.h>

#include <djvAssert.h>
#include <djvImage.h>
#include <djvListUtil.h>
#include <djvMemory.h>

#include <algorithm>

//------------------------------------------------------------------------------
// djvViewFileCacheItemPrivate
//------------------------------------------------------------------------------

struct djvViewFileCacheItemPrivate
{
    djvViewFileCacheItemPrivate(djvImage * in, const void * key, qint64 frame) :
        image(in),
        key  (key),
        frame(frame),
        count(1)
    {}

    QScopedPointer<djvImage> image;
    const void *             key;
    qint64                   frame;
    int                      count;
};

//------------------------------------------------------------------------------
// djvViewFileCacheItem
//------------------------------------------------------------------------------

namespace
{
int refAliveCount = 0;

} // namespace

djvViewFileCacheItem::djvViewFileCacheItem(
    djvImage *   image,
    const void * key,
    qint64       frame) :
    _p(new djvViewFileCacheItemPrivate(image, key, frame))
{
    ++refAliveCount;

    //DJV_DEBUG("djvViewFileCacheItem::djvViewFileCacheItem");
    //DJV_DEBUG_PRINT("alive = " << refAliveCount);
}

djvViewFileCacheItem::~djvViewFileCacheItem()
{
    --refAliveCount;

    //DJV_DEBUG("djvViewFileCacheItem::~djvViewFileCacheItem");
    //DJV_DEBUG_PRINT("alive = " << refAliveCount);

    delete _p;
}

djvImage * djvViewFileCacheItem::image()
{
    return _p->image.data();
}

const void * djvViewFileCacheItem::key() const
{
    return _p->key;
}

void djvViewFileCacheItem::resetKey()
{
    _p->key = 0;
}

qint64 djvViewFileCacheItem::frame() const
{
    return _p->frame;
}

void djvViewFileCacheItem::increment()
{
    ++_p->count;
}

void djvViewFileCacheItem::decrement()
{
    --_p->count;
}

int djvViewFileCacheItem::count() const
{
    return _p->count;
}

//------------------------------------------------------------------------------
// djvViewFileCachePrivate
//------------------------------------------------------------------------------

struct djvViewFileCachePrivate
{
    djvViewFileCachePrivate(djvViewContext * context) :
        maxByteCount  (static_cast<quint64>(
            context->filePrefs()->cacheSize() * djvMemory::gigabyte)),
        cacheByteCount(0),
        context       (context)
    {}
    
    djvViewFileCacheItemList items;
    quint64                  maxByteCount;
    quint64                  cacheByteCount;
    djvViewContext *         context;
};

//------------------------------------------------------------------------------
// djvViewFileCache
//------------------------------------------------------------------------------

djvViewFileCache::djvViewFileCache(djvViewContext * context, QObject * parent) :
    QObject(parent),
    _p(new djvViewFileCachePrivate(context))
{
    //DJV_DEBUG("djvViewFileCache::djvViewFileCache");
    
    connect(
        context->filePrefs(),
        SIGNAL(cacheChanged(bool)),
        SLOT(cacheCallback(bool)));   
    connect(
        context->filePrefs(),
        SIGNAL(cacheSizeChanged(double)),
        SLOT(cacheSizeCallback(double)));
}

djvViewFileCache::~djvViewFileCache()
{
    //DJV_DEBUG("djvViewFileCache::~djvViewFileCache");

    //debug();

    // Cleanup.
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        delete _p->items[i];
    }
    
    delete _p;
}

djvViewFileCacheItem * djvViewFileCache::create(
    djvImage *   image,
    const void * key,
    qint64       frame)
{
    //DJV_DEBUG("djvViewFileCache::create");
    //DJV_DEBUG_PRINT("image = " << *image);
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    //DJV_DEBUG_PRINT("frame = " << frame);

    //debug();

    // Create a new reference.
    djvViewFileCacheItem * out = new djvViewFileCacheItem(image, key, frame);

    // Add the reference to the end of the list.
    _p->items += out;

    // Update the cache size.
    _p->cacheByteCount += out->image()->dataByteCount();
    if (_p->cacheByteCount > _p->maxByteCount)
    {
        purge();
    }
    Q_EMIT cacheChanged();
    //debug();
    
    return out;
}

djvViewFileCacheItem * djvViewFileCache::get(const void * key, qint64 frame)
{
    //DJV_DEBUG("djvViewFileCache::get");
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    //DJV_DEBUG_PRINT("frame = " << frame);
    
    //debug();

    // Search for the requested item.
    djvViewFileCacheItem * item = 0;
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->items[i]->key() && frame == _p->items[i]->frame())
        {
            // Found it.
            item = _p->items[i];

            // Increment the reference count.
            item->increment();

            // Move the item to the end of the list.
            _p->items.remove(i);
            _p->items.push_back(item);
            
            break;
        }
    }
    //debug();

    //DJV_DEBUG_PRINT("out = " <<
    //  (out ? reinterpret_cast<qint64>(out->key()) : 0));
    return item;
}

bool djvViewFileCache::contains(const void * key, qint64 frame) const
{
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->items[i]->key() && frame == _p->items[i]->frame())
        {
            return true;
        }
    }
    return false;
}

void djvViewFileCache::del(const void * key)
{
    //DJV_DEBUG("djvViewFileCache::del");
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    
    debug();

    // Search for all items that match the given key.
    for (int i = _p->items.count() - 1; i >= 0; --i)
    {
        djvViewFileCacheItem * item = _p->items[i];
        if (key == item->key())
		{
            // If the item has a reference count of zero we can delete it now,
            // otherwise we will have to wait and delete it later.
            if (! item->count())
            {
                removeItem(i);
            }
            else
            {
                item->resetKey();
            }
        }
    }

    // Emit a signal that the cache has changed.
    Q_EMIT cacheChanged();

    debug();
}

void djvViewFileCache::del(const void * key, qint64 frame)
{
    //DJV_DEBUG("djvViewFileCache::del");
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    //DJV_DEBUG_PRINT("frame = " << frame);
    
    debug();

    // Search for all items that match the given key and frame.
    for (int i = _p->items.count() - 1; i >= 0; --i)
    {
        djvViewFileCacheItem * item = _p->items[i];
        if (key == item->key() && frame == item->frame())
        {
            // If the item has a reference count of zero we can delete it now,
            // otherwise we will have to wait and delete it later.
            if (! item->count())
            {
                removeItem(i);
            }
            else
            {
                item->resetKey();
            }
        }
    }

    // Emit a signal that the cache has changed.
    Q_EMIT cacheChanged();

    debug();
}

void djvViewFileCache::clear()
{
    //DJV_DEBUG("djvViewFileCache::clear");

    debug();

    // Delete all the items with a reference count of zero.
    for (int i = _p->items.count() - 1; i >= 0; --i)
    {
        if (! _p->items[i]->count())
        {
            removeItem(i);
        }
    }

    // Emit a signal that the cache has changed.
    Q_EMIT cacheChanged();

    debug();
}

djvViewFileCacheItemList djvViewFileCache::items(const void * key)
{
    djvViewFileCacheItemList items;
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->items[i]->key())
        {
            items.append(_p->items[i]);
        }
    }
    return items;
}

namespace
{

bool compare(qint64 a, qint64 b)
{
    return a < b;
}

} // namespace

djvFrameList djvViewFileCache::frames(const void * key)
{
    djvFrameList frames;
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->items[i]->key())
        {
            frames.append(_p->items[i]->frame());
        }
    }
    qSort(frames.begin(), frames.end(), compare);
    return frames;
}

double djvViewFileCache::maxSize() const
{
    return _p->maxByteCount / static_cast<double>(djvMemory::gigabyte);
}

quint64 djvViewFileCache::maxByteCount() const
{
    return _p->maxByteCount;
}

double djvViewFileCache::size(const void * key) const
{
    quint64 size = 0;
    const int count = _p->items.count();
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->items[i]->key())
        {
            size += _p->items[i]->image()->dataByteCount();
        }    
    }
    return size / static_cast<double>(djvMemory::gigabyte);
}

double djvViewFileCache::size() const
{
    return _p->cacheByteCount / static_cast<double>(djvMemory::gigabyte);
}

quint64 djvViewFileCache::byteCount() const
{
    return _p->cacheByteCount;
}

const QVector<double> & djvViewFileCache::sizeDefaults()
{
    static const QVector<double> data = QVector<double>() <<
        1.0 <<
        2.0 <<
        3.0 <<
        6.0 <<
        12.0 <<
        24.0 <<
        48.0 <<
        96.0;
    return data;
}

const QStringList & djvViewFileCache::sizeLabels()
{
    static const QStringList data = QStringList() <<
        "1" <<
        "2" <<
        "3" <<
        "6" <<
        "12" <<
        "24" <<
        "48" <<
        "96";
    return data;
}

void djvViewFileCache::debug()
{
    /*DJV_DEBUG("djvViewFileCache::debug");
    DJV_DEBUG_PRINT("items = " << _p->items.count());
    DJV_DEBUG_PRINT("cache max = " << maxSize());
    DJV_DEBUG_PRINT("cache size = " << size());

	for (int i = 0; i < _p->items.count(); ++i)
	{
        DJV_DEBUG_PRINT(
            "item (count = " << _p->items[i]->count() << ") = " <<
            reinterpret_cast<qint64>(_p->items[i]->key()) << " " <<
            _p->items[i]->frame());
	}*/
}

void djvViewFileCache::setMaxSize(double size)
{
    //DJV_DEBUG("djvViewFileCache::setMaxSize");
    //DJV_DEBUG_PRINT("size = " << size);

    //debug();

    _p->maxByteCount = static_cast<quint64>(size * djvMemory::gigabyte);
    //if (_p->cacheByteCount > _p->maxByteCount)
        purge();

    //debug();
}

void djvViewFileCache::removeItem(int index)
{
    djvViewFileCacheItem * item = _p->items[index];
    _p->items.remove(index);
    _p->cacheByteCount -= item->image()->dataByteCount();
    delete item;
}

void djvViewFileCache::purge()
{
    //DJV_DEBUG("djvViewFileCache::purge");

    debug();

    // Delete as many items as possible to bring the cache size below
    // the maximum size.
    QVector<int> itemsToDelete;
    const int count = _p->items.count();
    quint64 byteCountTmp = _p->cacheByteCount;
    for (int i = 0; i < count; ++i)
    {
        if (! _p->items[i]->count() && byteCountTmp > _p->maxByteCount)
        {
            itemsToDelete += i;
            byteCountTmp -= _p->items[i]->image()->dataByteCount();
        }
    }
    //DJV_DEBUG_PRINT("zombies = " << zombies.count());

    for (int i = itemsToDelete.count() - 1; i >= 0; --i)
    {
        removeItem(itemsToDelete[i]);
    }

    // Emit a signal that the cache has changed.
    Q_EMIT cacheChanged();

    debug();
}

void djvViewFileCache::cacheCallback(bool cache)
{
    if (! cache)
    {
        clear();
    }
}

void djvViewFileCache::cacheSizeCallback(double size)
{
	setMaxSize(size);
}
