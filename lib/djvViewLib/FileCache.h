//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/ViewLib.h>

#include <djvCore/Sequence.h>
#include <djvCore/Time.h>
#include <djvCore/Util.h>

#include <QObject>

#include <memory>
#include <map>

namespace djv
{
    namespace AV
    {
        class Image;
    
    } // namespace AV

    namespace ViewLib
    {
        class ViewContext;

        struct FileCacheKey
        {
            FileCacheKey();
            FileCacheKey(void * window, qint64 frame);

            void * window = nullptr;
            qint64 frame = 0;
            ::time_t timestamp = 0;

            bool operator < (const FileCacheKey &) const;
        };

        //! This class provides the file cache.
        class FileCache : public QObject
        {
            Q_OBJECT

        public:
            explicit FileCache(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~FileCache() override;

            //! Get whether the cache contains an item.
            bool hasItem(const FileCacheKey &);

            //! Get an item from the cache.
            std::shared_ptr<AV::Image> item(const FileCacheKey &) const;

            //! Add an item to the cache.
            void addItem(const FileCacheKey &, const std::shared_ptr<AV::Image> &);

            //! Remove all items with a zero reference count that match the given window.
            void clearItems(void *);

            //! Remove all items with a zero reference count.
            void clear();

            //! Remove an item.
            void removeItem(const FileCacheKey &);

            //! Get the list of items that match the given window.
            std::vector<std::shared_ptr<AV::Image> > items(void *);

            //! Get the list of frames that match the given window. The frames are sorted in
            //! ascending order.
            Core::FrameList frames(void *);

            //! Get the maximum cache size in gigabytes.
            float maxSizeGB() const;

            //! Get the maximum cache size in bytes.
            quint64 maxSizeBytes() const;

            //! Get the current size in gigabytes for the given window.
            float currentSizeGB(void *) const;

            //! Get the current cache size in gigabytes.
            float currentSizeGB() const;

            //! Get the current cache size in bytes.
            quint64 currentSizeBytes() const;

            //! Get the cache size defaults in gigabytes.
            static const QVector<float> & sizeGBDefaults();

            //! Print debugging information.
            void debug();

        public Q_SLOTS:
            //! Set the maximum cache size in gigabytes.
            void setMaxSizeGB(float);

        Q_SIGNALS:
            //! This signal is emitted when the cache is modified.
            void cacheChanged();

        private Q_SLOTS:
            void cacheEnabledCallback(bool);
            void cacheSizeGBCallback(float);

        private:
            void removeItem(int index);

            // Delete null references only if the cache size exceeds the maximum.
            void purge();

            DJV_PRIVATE_COPY(FileCache);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
