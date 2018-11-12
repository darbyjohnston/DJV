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

#include <djvAV/AudioData.h>

namespace djv
{
    namespace AV
    {
        AudioInfo::AudioInfo()
        {}

        AudioInfo::AudioInfo(size_t channels, Audio::TYPE type, size_t sampleRate, size_t sampleCount) :
            channels(channels),
            type(type),
            sampleRate(sampleRate),
            sampleCount(sampleCount)
        {}

        bool AudioInfo::operator == (const AudioInfo & other) const
        {
            return
                channels == other.channels &&
                type == other.type &&
                sampleRate == other.sampleRate &&
                sampleCount == other.sampleCount;
        }

        bool AudioInfo::operator != (const AudioInfo & other) const
        {
            return !(*this == other);
        }

        AudioData::AudioData()
        {}

        AudioData::AudioData(const AudioInfo & info)
        {
            set(info);
        }

        AudioData::~AudioData()
        {}

        void AudioData::set(const AudioInfo & info)
        {
            if (info == _info)
                return;
            _info = info;
            _data.resize(_info.sampleCount * Audio::byteCount(_info.type));
        }

        namespace
        {
            template<typename U, typename V>
            void _convert(const U * value, V * out, size_t size)
            {
                for (size_t i = 0; i < size; ++i, ++value, ++out)
                {
                    Audio::convert(*value, *out);
                }
            }
        }

        AudioData AudioData::convert(const AudioData & data, Audio::TYPE type)
        {
            const size_t sampleCount = data.sampleCount();
            AudioData out(AudioInfo(data.channels(), type, data.sampleRate(), sampleCount));
            switch (data.type())
            {
            case Audio::U8:
                switch (type)
                {
                case Audio::U8:  memcpy(out.data(), data.data(), sampleCount * Audio::byteCount(type)); break;
                case Audio::S16: _convert(reinterpret_cast<const Audio::U8_T *>(data.data()), reinterpret_cast<Audio::S16_T *>(out.data()), sampleCount); break;
                case Audio::S32: _convert(reinterpret_cast<const Audio::U8_T *>(data.data()), reinterpret_cast<Audio::S32_T *>(out.data()), sampleCount); break;
                case Audio::F32: _convert(reinterpret_cast<const Audio::U8_T *>(data.data()), reinterpret_cast<Audio::F32_T *>(out.data()), sampleCount); break;
                default: break;
                }
                break;
            case Audio::S16:
                switch (type)
                {
                case Audio::U8:  _convert(reinterpret_cast<const Audio::S16_T *>(data.data()), reinterpret_cast<Audio::U8_T *> (out.data()), sampleCount); break;
                case Audio::S16: memcpy(out.data(), data.data(), data.sampleCount() * Audio::byteCount(type)); break;
                case Audio::S32: _convert(reinterpret_cast<const Audio::S16_T *>(data.data()), reinterpret_cast<Audio::S32_T *>(out.data()), sampleCount); break;
                case Audio::F32: _convert(reinterpret_cast<const Audio::S16_T *>(data.data()), reinterpret_cast<Audio::F32_T *>(out.data()), sampleCount); break;
                default: break;
                }
                break;
            case Audio::S32:
                switch (type)
                {
                case Audio::U8:  _convert(reinterpret_cast<const Audio::S32_T *>(data.data()), reinterpret_cast<Audio::U8_T *> (out.data()), sampleCount); break;
                case Audio::S16: _convert(reinterpret_cast<const Audio::S32_T *>(data.data()), reinterpret_cast<Audio::S16_T *>(out.data()), sampleCount); break;
                case Audio::S32: memcpy(out.data(), data.data(), data.sampleCount() * Audio::byteCount(type)); break;
                case Audio::F32: _convert(reinterpret_cast<const Audio::S32_T *>(data.data()), reinterpret_cast<Audio::F32_T *>(out.data()), sampleCount); break;
                default: break;
                }
                break;
            case Audio::F32:
                switch (type)
                {
                case Audio::U8:  _convert(reinterpret_cast<const Audio::F32_T *>(data.data()), reinterpret_cast<Audio::U8_T *> (out.data()), sampleCount); break;
                case Audio::S16: _convert(reinterpret_cast<const Audio::F32_T *>(data.data()), reinterpret_cast<Audio::S16_T *>(out.data()), sampleCount); break;
                case Audio::S32: _convert(reinterpret_cast<const Audio::F32_T *>(data.data()), reinterpret_cast<Audio::S32_T *>(out.data()), sampleCount); break;
                case Audio::F32: memcpy(out.data(), data.data(), data.sampleCount() * Audio::byteCount(type)); break;
                default: break;
                }
                break;
            default: break;
            }
            return out;
        }

        namespace
        {
            template<typename U>
            void _planarInterleave(const U * value, U * out, size_t channels, size_t size)
            {
                const size_t planeSize = size / channels;
                for (size_t c = 0; c < channels; ++c)
                {
                    const U * inP = value + c * planeSize;
                    U * outP = out + c;
                    for (size_t i = 0; i < planeSize; ++i, ++inP, outP += channels)
                    {
                        *outP = *inP;
                    }
                }
            }
        }

        AudioData AudioData::planarInterleave(const AudioData & data)
        {
            AudioData out(data.info());
            const size_t channels = data.channels();
            const size_t sampleCount = data.sampleCount();
            switch (data.type())
            {
            case Audio::U8: 
                _planarInterleave(reinterpret_cast<const Audio::U8_T *> (data.data()), reinterpret_cast<Audio::U8_T *> (out.data()), channels, sampleCount);
                break;
            case Audio::S16:
                _planarInterleave(reinterpret_cast<const Audio::S16_T *>(data.data()), reinterpret_cast<Audio::S16_T *>(out.data()), channels, sampleCount);
                break;
            case Audio::S32:
                _planarInterleave(reinterpret_cast<const Audio::S32_T *>(data.data()), reinterpret_cast<Audio::S32_T *>(out.data()), channels, sampleCount);
                break;
            case Audio::F32:
                _planarInterleave(reinterpret_cast<const Audio::F32_T *>(data.data()), reinterpret_cast<Audio::F32_T *>(out.data()), channels, sampleCount);
                break;
            default: break;
            }
            return out;
        }

        namespace
        {
            template<typename U>
            void _planarDeinterleave(const U * value, U * out, size_t channels, size_t size)
            {
                const size_t planeSize = size / channels;
                for (size_t c = 0; c < channels; ++c)
                {
                    const U * inP = value + c;
                    U * outP = out + c * planeSize;
                    for (size_t i = 0; i < planeSize; ++i, inP += channels, ++outP)
                    {
                        *outP = *inP;
                    }
                }
            }
        }

        AudioData AudioData::planarDeinterleave(const AudioData & data)
        {
            AudioData out(data.info());
            const size_t channels = data.channels();
            const size_t sampleCount = data.sampleCount();
            switch (data.type())
            {
            case Audio::U8:
                _planarDeinterleave(reinterpret_cast<const Audio::U8_T *> (data.data()), reinterpret_cast<Audio::U8_T *> (out.data()), channels, sampleCount);
                break;
            case Audio::S16:
                _planarDeinterleave(reinterpret_cast<const Audio::S16_T *>(data.data()), reinterpret_cast<Audio::S16_T *>(out.data()), channels, sampleCount);
                break;
            case Audio::S32:
                _planarDeinterleave(reinterpret_cast<const Audio::S32_T *>(data.data()), reinterpret_cast<Audio::S32_T *>(out.data()), channels, sampleCount);
                break;
            case Audio::F32:
                _planarDeinterleave(reinterpret_cast<const Audio::F32_T *>(data.data()), reinterpret_cast<Audio::F32_T *>(out.data()), channels, sampleCount);
                break;
            default: break;
            }
            return out;
        }

        bool AudioData::operator == (const AudioData & other) const
        {
            return
                _info == other._info &&
                0 == memcmp(data(), other.data(), byteCount());
        }

        bool AudioData::operator != (const AudioData & other) const
        {
            return !(*this == other);
        }

    } // namespace AV

} // namespace djv
