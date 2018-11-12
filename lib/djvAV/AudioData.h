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

#include <djvAV/Audio.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        //! This struct provides audio data information.
        struct AudioInfo
        {
            AudioInfo();
            AudioInfo(size_t channels, Audio::TYPE type, size_t sampleRate, size_t sampleCount);

            size_t       channels    = 0;
            Audio::TYPE  type        = Audio::TYPE_NONE;
            size_t       sampleRate  = 0;
            uint64_t     sampleCount = 0;

            inline bool isValid() const;
            inline uint64_t byteCount() const;

            bool operator == (const AudioInfo &) const;
            bool operator != (const AudioInfo &) const;
        };

        //! This class provides audio data.
        class AudioData
        {
            Q_GADGET

        public:
            AudioData();
            AudioData(const AudioInfo &);
            ~AudioData();

            void set(const AudioInfo &);
            void zero();

            inline const AudioInfo & info() const;
            inline size_t channels() const;
            inline Audio::TYPE type() const;
            inline size_t sampleRate() const;
            inline uint64_t sampleCount() const;

            inline bool isValid() const;
            inline uint64_t byteCount() const;

            inline uint8_t * data();
            inline const uint8_t * data() const;
            inline uint8_t * data(uint64_t offset);
            inline const uint8_t * data(uint64_t offset) const;

            static AudioData convert(const AudioData &, Audio::TYPE);

            static AudioData planarInterleave(const AudioData &);
            static AudioData planarDeinterleave(const AudioData &);

            bool operator == (const AudioData &) const;
            bool operator != (const AudioData &) const;

        private:
            AudioInfo _info;
            std::vector<uint8_t> _data;
        };

    } // namespace AV
} // namespace djv

#include <djvAV/AudioDataInline.h>
