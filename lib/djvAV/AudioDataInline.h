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

namespace djv
{
    namespace AV
    {
        inline size_t AudioDataInfo::getChannels() const
        {
            return _channels;
        }

        inline Audio::Type AudioDataInfo::getType() const
        {
            return _type;
        }

        inline size_t AudioDataInfo::getSampleRate() const
        {
            return _sampleRate;
        }

        inline bool AudioDataInfo::isValid() const
        {
            return _type != Audio::Type::None;
        }

        inline const AudioDataInfo & AudioData::getInfo() const
        {
            return _info;
        }

        inline size_t AudioData::getChannels() const
        {
            return _info.getChannels();
        }

        inline Audio::Type AudioData::getType() const
        {
            return _info.getType();
        }

        inline size_t AudioData::getSampleRate() const
        {
            return _info.getSampleRate();
        }

        inline size_t AudioData::getSampleCount() const
        {
            return _sampleCount;
        }

        inline bool AudioData::isValid() const
        {
            return _info.isValid();
        }

        inline size_t AudioData::getByteCount() const
        {
            return _sampleCount * Audio::getByteCount(_info.getType());
        }

        inline uint8_t * AudioData::getData()
        {
            return _data.data();
        }

        inline const uint8_t * AudioData::getData() const
        {
            return _data.data();
        }

        inline uint8_t * AudioData::getData(size_t offset)
        {
            return _data.data();
        }

        inline const uint8_t * AudioData::getData(size_t offset) const
        {
            return _data.data();
        }

    } // namespace AV
} // namespace djv
