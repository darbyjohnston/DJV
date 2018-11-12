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

namespace djv
{
    namespace AV
    {
        inline bool AudioInfo::isValid() const
        {
            return type != Audio::TYPE_NONE;
        }

        inline uint64_t AudioInfo::byteCount() const
        {
            return sampleCount * Audio::byteCount(type);
        }

        inline const AudioInfo & AudioData::info() const
        {
            return _info;
        }

        inline size_t AudioData::channels() const
        {
            return _info.channels;
        }

        inline Audio::TYPE AudioData::type() const
        {
            return _info.type;
        }

        inline size_t AudioData::sampleRate() const
        {
            return _info.sampleRate;
        }

        inline uint64_t AudioData::sampleCount() const
        {
            return _info.sampleCount;
        }

        inline bool AudioData::isValid() const
        {
            return _info.isValid();
        }

        inline uint8_t * AudioData::data()
        {
            return _data.data();
        }

        inline const uint8_t * AudioData::data() const
        {
            return _data.data();
        }

        inline uint8_t * AudioData::data(uint64_t offset)
        {
            return _data.data();
        }

        inline const uint8_t * AudioData::data(uint64_t offset) const
        {
            return _data.data();
        }

        inline uint64_t AudioData::byteCount() const
        {
            return _info.byteCount();
        }

    } // namespace AV
} // namespace djv
