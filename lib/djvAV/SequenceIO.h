// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IO.h>

#include <djvCore/Frame.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This class provides an interface for reading sequences.
            class ISequenceRead : public IRead
            {
                DJV_NON_COPYABLE(ISequenceRead);

            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const ReadOptions&,
                    const std::shared_ptr<Core::TextSystem>&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);
                ISequenceRead();

            public:
                virtual ~ISequenceRead() override = 0;

                bool isRunning() const override;
                std::future<Info> getInfo() override;
                void seek(int64_t, Direction) override;
                bool hasCache() const override { return true; }

            protected:
                virtual Info _readInfo(const std::string & fileName) = 0;
                virtual std::shared_ptr<Image::Image> _readImage(const std::string & fileName) = 0;
                void _finish();

                Core::Time::Speed _speed;
                Core::Frame::Sequence _sequence;

            private:
                bool _hasWork() const;
                size_t _getQueueCount(size_t threadCount) const;
                struct Future;
                std::future<Future> _getFuture(Core::Frame::Number, std::string fileName);
                size_t _readQueue(size_t count, bool loop, bool cacheEnabled);
                void _readCache(size_t count, const AV::IO::InOutPoints&);

                DJV_PRIVATE();
            };

            //! This class provides an interface for writing sequences.
            class ISequenceWrite : public IWrite
            {
                DJV_NON_COPYABLE(ISequenceWrite);

            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const Info &,
                    const WriteOptions&,
                    const std::shared_ptr<Core::TextSystem>&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);
                ISequenceWrite();

            public:
                virtual ~ISequenceWrite() override = 0;

                bool isRunning() const override;

            protected:
                virtual Image::Type _getImageType(Image::Type) const;
                virtual Image::Layout _getImageLayout() const;
                virtual void _write(const std::string & fileName, const std::shared_ptr<Image::Image> &) = 0;
                void _finish();

                Info _info;
                Image::Info _imageInfo;

            private:
                DJV_PRIVATE();
            };

            //! This class provides an interface for sequence I/O plugins.
            class ISequencePlugin : public IPlugin
            {
            public:
                virtual ~ISequencePlugin() = 0;

                bool canSequence() const override;
            };

        } // namespace IO
    } // namespace AV
} // namespace djv
