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

//! \file djvViewFileCache.cpp

#include <djvViewFileCache.h>

#include <djvViewApplication.h>
#include <djvViewFilePrefs.h>

#include <djvAssert.h>
#include <djvImage.h>
#include <djvListUtil.h>
#include <djvMemory.h>

#include <algorithm>

namespace
{

typedef QVector<djvViewFileCacheRef *> RefList;

} // namespace

//------------------------------------------------------------------------------
// djvViewFileCacheRef::P
//------------------------------------------------------------------------------

struct djvViewFileCacheRef::P
{
    P(djvImage * in, const void * key, qint64 frame) :
        image   (in),
        key     (key),
        frame   (frame),
        refCount(1)
    {}

    QScopedPointer<djvImage> image;
    const void *             key;
    qint64                   frame;
    int                      refCount;
};

//------------------------------------------------------------------------------
// djvViewFileCacheRef
//------------------------------------------------------------------------------

namespace
{

int refAliveCount = 0;

} // namespace

djvViewFileCacheRef::djvViewFileCacheRef(djvImage * in, const void * key, qint64 frame) :
    _p(new P(in, key, frame))
{
    ++refAliveCount;

    //DJV_DEBUG("djvViewFileCacheRef::djvViewFileCacheRef");
    //DJV_DEBUG_PRINT("alive = " << refAliveCount);
}

djvViewFileCacheRef::~djvViewFileCacheRef()
{
    --refAliveCount;

    //DJV_DEBUG("djvViewFileCacheRef::~djvViewFileCacheRef");
    //DJV_DEBUG_PRINT("alive = " << refAliveCount);

    delete _p;
}

djvImage * djvViewFileCacheRef::image()
{
    return _p->image.data();
}

const void * djvViewFileCacheRef::key() const
{
    return _p->key;
}

void djvViewFileCacheRef::setKey(const void * in)
{
    _p->key = in;
}

qint64 djvViewFileCacheRef::frame() const
{
    return _p->frame;
}

void djvViewFileCacheRef::refInc()
{
    ++_p->refCount;
}

void djvViewFileCacheRef::refDec()
{
    --_p->refCount;
}

int djvViewFileCacheRef::refCount() const
{
    return _p->refCount;
}

//------------------------------------------------------------------------------
// djvViewFileCache::P
//------------------------------------------------------------------------------

struct djvViewFileCache::P
{
    P() :
        maxSize  (static_cast<quint64>(
        djvViewFilePrefs::global()->cacheSize() * djvMemory::gigabyte)),
        cacheSize(0)
    {}
    
    RefList refs;
    quint64 maxSize;
    quint64 cacheSize;
};

//------------------------------------------------------------------------------
// djvViewFileCache
//------------------------------------------------------------------------------

djvViewFileCache::djvViewFileCache(QObject * parent) :
    QObject(parent),
    _p(new P)
{
    //DJV_DEBUG("djvViewFileCache::djvViewFileCache");
    
    connect(
        djvViewFilePrefs::global(),
        SIGNAL(cacheEnabledChanged(bool)),
        SLOT(cacheEnabledCallback(bool)));
    
    connect(
        djvViewFilePrefs::global(),
        SIGNAL(cacheSizeChanged(double)),
        SLOT(cacheSizeCallback(double)));
}

djvViewFileCache::~djvViewFileCache()
{
    //DJV_DEBUG("djvViewFileCache::~djvViewFileCache");

    //debug();

    // Cleanup.
    
    const int count = _p->refs.count();
    
    for (int i = 0; i < count; ++i)
    {
        delete _p->refs[i];
    }
    
    delete _p;
}

djvViewFileCacheRef * djvViewFileCache::create(
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

    djvViewFileCacheRef * out = new djvViewFileCacheRef(image, key, frame);

    // Add the reference to the end of the list.

    _p->refs += out;

    // Update the cache size.

    _p->cacheSize += out->image()->dataByteCount();

    if (_p->cacheSize > _p->maxSize)
    {
        purge();
    }

    Q_EMIT cacheChanged();

    //debug();

    return out;
}

djvViewFileCacheRef * djvViewFileCache::get(const void * key, qint64 frame)
{
    //DJV_DEBUG("djvViewFileCache::get");
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    //DJV_DEBUG_PRINT("frame = " << frame);
    
    //debug();

    djvViewFileCacheRef * out = 0;

    const int count = _p->refs.count();

    for (int i = 0; i < count; ++i)
    {
        if (key == _p->refs[i]->key() && frame == _p->refs[i]->frame())
        {
            // Found the reference.

            out = _p->refs[i];

            // Increment the reference count.

            out->refInc();

            // Move the reference to the end of the list.

            _p->refs.remove(i);

            _p->refs.push_back(out);

            break;
        }
    }

    //debug();

    //DJV_DEBUG_PRINT("out = " <<
    //  (out ? reinterpret_cast<qint64>(out->key()) : 0));

    return out;
}

void djvViewFileCache::del(const void * key)
{
    //DJV_DEBUG("djvViewFileCache::del");
    //DJV_DEBUG_PRINT("key = " << reinterpret_cast<qint64>(key));
    
    debug();

    // Delete any null references that match the key.
	
    for (int i = _p->refs.count() - 1; i >= 0; --i)
    {
        if (! _p->refs[i]->refCount() && key == _p->refs[i]->key())
		{
			remove(i);
        }
    }

    // Initialize any remaining references that match the key; they will be
    // cleaned up later.
    
    const int count = _p->refs.count();
    
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->refs[i]->key())
        {
            _p->refs[i]->setKey(0);
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

    // Initialize any remaining references that match the key; they will be
    // cleaned up later.
    
    const int count = _p->refs.count();
    
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->refs[i]->key() && frame == _p->refs[i]->frame())
        {
            _p->refs[i]->setKey(0);
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

    // Delete all of the null references.
    
    for (int i = _p->refs.count() - 1; i >= 0; --i)
    {
        if (! _p->refs[i]->refCount())
        {
			remove(i);
        }
    }

    // Emit a signal that the cache has changed.

    Q_EMIT cacheChanged();

    debug();
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
    djvFrameList out;
    
    const int count = _p->refs.count();
    
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->refs[i]->key())
        {
            out += _p->refs[i]->frame();
        }
    }
    
    qSort(out.begin(), out.end(), compare);

    return out;
}

double djvViewFileCache::maxSize() const
{
    return _p->maxSize / static_cast<double>(djvMemory::gigabyte);
}

double djvViewFileCache::size(const void * key) const
{
    quint64 size = 0;
    
    const int count = _p->refs.count();
    
    for (int i = 0; i < count; ++i)
    {
        if (key == _p->refs[i]->key())
        {
            size += _p->refs[i]->image()->dataByteCount();
        }    
    }

    return size / static_cast<double>(djvMemory::gigabyte);
}

double djvViewFileCache::size() const
{
    return _p->cacheSize / static_cast<double>(djvMemory::gigabyte);
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
    DJV_DEBUG_PRINT("refs = " << _p->refs.count());
    DJV_DEBUG_PRINT("cache max = " << maxSize());
    DJV_DEBUG_PRINT("cache size = " << size());

	for (int i = 0; i < _p->refs.count(); ++i)
	{
        DJV_DEBUG_PRINT(
            "item (ref = " << _p->refs[i]->refCount() << ") = " <<
            reinterpret_cast<qint64>(_p->refs[i]->key()) << " " <<
            _p->refs[i]->frame());
	}*/
}

djvViewFileCache * djvViewFileCache::global()
{
    static djvViewFileCache * cache = 0;

    if (! cache)
    {
        cache = new djvViewFileCache(DJV_VIEW_APP);
    }

    return cache;
}

void djvViewFileCache::setMaxSize(double size)
{
    //DJV_DEBUG("djvViewFileCache::setMaxSize");
    //DJV_DEBUG_PRINT("size = " << size);

    //debug();

    _p->maxSize = static_cast<quint64>(size * djvMemory::gigabyte);

    //if (_p->cacheSize > _p->maxSize)
    purge();

    //debug();
}

void djvViewFileCache::remove(int index)
{
	djvViewFileCacheRef * ref = _p->refs[index];
	
	_p->refs.remove(index);

	_p->cacheSize -= ref->image()->dataByteCount();

	delete ref;
}

void djvViewFileCache::purge()
{
    //DJV_DEBUG("djvViewFileCache::purge");

    debug();

    // Delete as many null references as needed to bring the cache size below
    // the maximum size.
	
    QVector<int> zombies;
	
	const int count = _p->refs.count();
	
	quint64 cacheSizeTmp = _p->cacheSize;
	
	for (int i = 0; i < count; ++i)
	{
        if (! _p->refs[i]->refCount() && cacheSizeTmp > _p->maxSize)
        {
			zombies += i;
			
			cacheSizeTmp -= _p->refs[i]->image()->dataByteCount();
		}
	}
	
	//DJV_DEBUG_PRINT("zombies = " << zombies.count());
    
    for (int i = zombies.count() - 1; i >= 0; --i)
    {
		remove(zombies[i]);
    }

    // Emit a signal that the cache has changed.

    Q_EMIT cacheChanged();

    debug();
}

void djvViewFileCache::cacheEnabledCallback(bool cache)
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
