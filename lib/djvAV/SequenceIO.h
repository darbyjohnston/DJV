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
                    const std::string & fileName,
                    const ReadOptions&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);
                ISequenceRead();

            public:
                virtual ~ISequenceRead() override = 0;

                bool isRunning() const override;
                std::future<Info> getInfo() override;
                void seek(Core::Time::Timestamp) override;

            protected:
                virtual Info _readInfo(const std::string & fileName) = 0;
                virtual std::shared_ptr<Image::Image> _readImage(const std::string & fileName) = 0;
                void _finish();

                Core::Time::Speed _speed;
                Core::Time::Timestamp _duration = 0;
                std::vector<Core::Frame::Number> _frames;

            private:
                DJV_PRIVATE();
            };

            //! This class provides an interface for writing sequences.
            class ISequenceWrite : public IWrite
            {
                DJV_NON_COPYABLE(ISequenceWrite);

            protected:
                void _init(
                    const std::string &,
                    const Info &,
                    const WriteOptions&,
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

        } // namespace IO
    } // namespace AV
} // namespace djv
