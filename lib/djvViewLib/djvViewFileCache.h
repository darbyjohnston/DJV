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

//! \file djvViewFileCache.h

#ifndef DJV_VIEW_FILE_CACHE_H
#define DJV_VIEW_FILE_CACHE_H

#include <djvViewLibExport.h>

#include <djvSequence.h>
#include <djvUtil.h>

#include <QObject>

class djvImage;

//! \addtogroup djvViewFile
//@{

//------------------------------------------------------------------------------
//! \class djvViewFileCacheRef
//!
//! This class provides a file cache reference.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFileCacheRef
{
public:

    //! Constructor.

    djvViewFileCacheRef(djvImage *, const void * key, qint64 frame);

    //! Destructor.

    ~djvViewFileCacheRef();

    //! Get the image.

    djvImage * image();

    //! Get the key.

    const void * key() const;

    //! Set the key.

    void setKey(const void *);

    //! Get the frame.

    qint64 frame() const;

    //! Increment the reference count.

    void refInc();

    //! Decrement the reference count.

    void refDec();

    //! Get the reference count.

    int refCount() const;

private:
    
    DJV_PRIVATE_COPY(djvViewFileCacheRef);
    DJV_PRIVATE_IMPLEMENTATION();
};

//------------------------------------------------------------------------------
//! \class djvViewFileCache
//!
//! This class provides the file cache.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewFileCache : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewFileCache(QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewFileCache();

    //! Create a cache reference.

    djvViewFileCacheRef * create(djvImage *, const void * key, qint64 frame);

    //! Get a cache reference.

    djvViewFileCacheRef * get(const void * key, qint64 frame);

    //! Delete all null references matching the given key.

    void del(const void * key);

    //! Delete all null references matching the given key and frame.

    void del(const void * key, qint64 frame);

    //! Delete all of the null references.

    void clear();

    //! Get a list of frames for the given key.

    djvFrameList frames(const void * key);

    //! Get the maximum cache size in gigabytes.

    double maxSize() const;

    //! Get the size in gigabytes for the given key.

    double size(const void * key) const;

    //! Get the cache size in gigabytes.

    double size() const;

    //! Get the cache size defaults.

    static const QVector<double> & sizeDefaults();

    //! Get the default cache size labels.

    static const QStringList & sizeLabels();

    //! Print debugging information.

    void debug();

    //! Get the global file cache.

    static djvViewFileCache * global();

public Q_SLOTS:

    //! Set the maximum cache size in gigabytes.

    void setMaxSize(double gigabytes);

Q_SIGNALS:

    //! This signal is emitted when the cache changes.

    void cacheChanged();

private Q_SLOTS:

    void cacheEnabledCallback(bool);
    void cacheSizeCallback(double);
    
private:

    void remove(int index);
    
    // Delete the null references only if the cache size exceeds the maximum.

    void purge();
    
    DJV_PRIVATE_COPY(djvViewFileCache);
    DJV_PRIVATE_IMPLEMENTATION();
};

//@} // djvViewFile

#endif // DJV_VIEW_FILE_CACHE_H

