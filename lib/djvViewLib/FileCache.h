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

#pragma once

#include <djvViewLib/Core.h>

#include <djvCore/Sequence.h>
#include <djvCore/Util.h>

#include <QObject>

#include <memory>

namespace djv
{
    namespace Graphics
    {
        class Image;
    
    } // namespace Graphics

    namespace ViewLib
    {
        class Context;

        //! \class FileCacheItem
        //!
        //! This class provides a file cache reference.
        class FileCacheItem
        {
        public:
            //! Constructor.
            //!
            //! \param image The image to be cached. Ownership of the image is passed
            //! to the cache item.
            //! \param key   A key to be associated with this cache item.
            //! \param frame The frame number associated with this cache item.
            FileCacheItem(
                Graphics::Image * image,
                const void *      key,
                qint64            frame);

            ~FileCacheItem();

            //! Get the image.
            Graphics::Image * image();

            //! Get the key.
            const void * key() const;

            //! Set the key to null.
            void resetKey();

            //! Get the frame.
            qint64 frame() const;

            //! Increment the reference count.
            void increment();

            //! Decrement the reference count.
            void decrement();

            //! Get the reference count.
            int count() const;

        private:
            DJV_PRIVATE_COPY(FileCacheItem);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        typedef QVector<FileCacheItem *> FileCacheItemList;

        //! \class djvViewFileCache
        //!
        //! This class provides the file cache.
        class FileCache : public QObject
        {
            Q_OBJECT

        public:
            explicit FileCache(Context *, QObject * parent = nullptr);
            ~FileCache() override;

            //! Create a new cache item. The reference count on the item is
            //! automatically set to one.
            FileCacheItem * create(Graphics::Image *, const void * key, qint64 frame);

            //! Get a cache item. The reference count on the item is automatically
            //! incremented.
            FileCacheItem * get(const void * key, qint64 frame);

            //! Get whether the cache contains the given item.
            bool contains(const void * key, qint64 frame) const;

            //! Delete all items matching the given key that have a reference count
            //! of zero.
            void del(const void * key);

            //! Delete all items matching the given key and frame that have a
            //! reference count of zero.
            void del(const void * key, qint64 frame);

            //! Delete all items that have a reference count of zero.
            void clear();

            //! Get the list of items for the given key.
            FileCacheItemList items(const void * key);

            //! Get the list of frames for the given key. The frames are sorted in
            //! ascending order.
            Core::FrameList frames(const void * key);

            //! Get the maximum cache size in gigabytes.
            float maxSize() const;

            //! Get the maximum cache size in bytes.
            quint64 maxByteCount() const;

            //! Get the size in gigabytes for the given key.
            float size(const void * key) const;

            //! Get the cache size in gigabytes.
            float size() const;

            //! Get the cache size in bytes.
            quint64 byteCount() const;

            //! Get the cache size defaults.
            static const QVector<float> & sizeDefaults();

            //! Get the default cache size labels.
            static const QStringList & sizeLabels();

            //! Print debugging information.
            void debug();

        public Q_SLOTS:
            //! Set the maximum cache size in gigabytes.
            void setMaxSize(float gigabytes);

        Q_SIGNALS:
            //! This signal is emitted when the cache changes.
            void cacheChanged();

        private Q_SLOTS:
            void cacheCallback(bool);
            void cacheSizeCallback(float);

        private:
            void removeItem(int index);

            // Delete the null references only if the cache size exceeds the maximum.
            void purge();

            DJV_PRIVATE_COPY(FileCache);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
