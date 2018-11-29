//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <AV/Audio.h>

namespace djv
{
    namespace AV
    {
        class AudioDataInfo
        {
            Q_GADGET

        public:
            AudioDataInfo();
            AudioDataInfo(size_t channels, Audio::Type, size_t sampleRate);

            inline size_t getChannels() const;
            inline Audio::Type getType() const;
            inline size_t getSampleRate() const;
            inline bool isValid() const;

            bool operator == (const AudioDataInfo &) const;
            bool operator != (const AudioDataInfo &) const;

        private:
            size_t      _channels   = 0;
            Audio::Type _type       = Audio::Type::None;
            size_t      _sampleRate = 0;
        };

        class AudioData
        {
            Q_GADGET
            DJV_NON_COPYABLE(AudioData);

        protected:
            void _init(const AudioDataInfo &, size_t sampleCount);
            AudioData();

        public:
            ~AudioData();

            static std::shared_ptr<AudioData> create(const AudioDataInfo &, size_t sampleCount);

            void zero();

            inline const AudioDataInfo & getInfo() const;
            inline size_t getChannels() const;
            inline Audio::Type getType() const;
            inline size_t getSampleRate() const;
            inline size_t getSampleCount() const;
            inline bool isValid() const;
            inline size_t getByteCount() const;

            inline uint8_t * getData();
            inline const uint8_t * getData() const;
            inline uint8_t * getData(size_t offset);
            inline const uint8_t * getData(size_t offset) const;

            static std::shared_ptr<AudioData> convert(const std::shared_ptr<AudioData> &, Audio::Type);

            static std::shared_ptr<AudioData> planarInterleave(const std::shared_ptr<AudioData> &);
            static std::shared_ptr<AudioData> planarDeinterleave(const std::shared_ptr<AudioData> &);

            bool operator == (const AudioData &) const;
            bool operator != (const AudioData &) const;

        private:
            AudioDataInfo _info;
            size_t _sampleCount = 0;
            std::vector<uint8_t> _data;
        };

    } // namespace AV
} // namespace djv

#include <AV/AudioDataInline.h>
