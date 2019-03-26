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

#include <djvAV/Audio.h>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            //! This struct provides audio data information.
            struct DataInfo
            {
                constexpr DataInfo();
                constexpr DataInfo(size_t channelCount, Type, size_t sampleRate);

                size_t channelCount = 0;
                Type   type = Audio::Type::None;
                size_t sampleRate = 0;
                
                constexpr bool isValid() const;

                bool operator == (const DataInfo &) const;
                bool operator != (const DataInfo &) const;
            };

            //! This struct provides audio data.
            class Data
            {
                DJV_NON_COPYABLE(Data);

            protected:
                void _init(const DataInfo &, size_t sampleCount);
                inline Data();

            public:
                static std::shared_ptr<Data> create(const DataInfo &, size_t sampleCount);

                void zero();

                inline const DataInfo & getInfo() const;
                inline size_t getChannelCount() const;
                inline Type getType() const;
                inline size_t getSampleRate() const;
                inline size_t getSampleCount() const;
                inline bool isValid() const;
                inline size_t getByteCount() const;

                inline uint8_t * getData();
                inline const uint8_t * getData() const;
                inline uint8_t * getData(size_t offset);
                inline const uint8_t * getData(size_t offset) const;

                static std::shared_ptr<Data> convert(const std::shared_ptr<Data> &, Type);

                static std::shared_ptr<Data> planarInterleave(const std::shared_ptr<Data> &);
                static void planarInterleave(const float **, float *, size_t size, size_t channelCount);
                static std::shared_ptr<Data> planarDeinterleave(const std::shared_ptr<Data> &);

                bool operator == (const Data &) const;
                bool operator != (const Data &) const;

            private:
                DataInfo _info;
                size_t _sampleCount = 0;
                std::vector<uint8_t> _data;
            };

        } // namespace Audio
    } // namespace AV
} // namespace djv

#include <djvAV/AudioDataInline.h>
