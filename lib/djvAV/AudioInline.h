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

#include <djvCore/Math.h>

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            inline uint8_t getByteCount(Type value)
            {
                const uint8_t data[] = { 0, 1, 2, 4, 4 };
                return data[static_cast<size_t>(value)];
            }

            inline Type getIntType(uint8_t value)
            {
                const Type data[] = { Type::None, Type::S8, Type::S16, Type::S32, Type::None };
                return value < sizeof(data) / sizeof(data[0]) ? data[value] : Type::None;
            }

            inline Type getFloatType(uint8_t value)
            {
                const Type data[] = { Type::None, Type::None, Type::None, Type::None, Type::F32, Type::F64 };
                return value < sizeof(data) / sizeof(data[0]) ? data[value] : Type::None;
            }

            /*inline ALenum getALType(uint8_t channels, Type type)
            {
                ALenum out = AL_NONE;
                switch (channels)
                {
                case 1:
                    switch (type)
                    {
                    case Type::U8:  out = AL_FORMAT_MONO8; break;
                    case Type::S16: out = AL_FORMAT_MONO16; break;
                    case Type::F32: out = AL_FORMAT_MONO_FLOAT32; break;
                    default: break;
                    }
                    break;
                case 2:
                    switch (type)
                    {
                    case Type::U8:  out = AL_FORMAT_STEREO8; break;
                    case Type::S16: out = AL_FORMAT_STEREO16; break;
                    case Type::F32: out = AL_FORMAT_STEREO_FLOAT32; break;
                    default: break;
                    }
                    break;
                case 6:
                    switch (type)
                    {
                    case Type::U8:  out = AL_FORMAT_51CHN8; break;
                    case Type::S16: out = AL_FORMAT_51CHN16; break;
                    case Type::F32: out = AL_FORMAT_51CHN32; break;
                    default: break;
                    }
                    break;
                case 7:
                    switch (type)
                    {
                    case Type::U8:  out = AL_FORMAT_61CHN8; break;
                    case Type::S16: out = AL_FORMAT_61CHN16; break;
                    case Type::F32: out = AL_FORMAT_61CHN32; break;
                    default: break;
                    }
                    break;
                case 8:
                    switch (type)
                    {
                    case Type::U8:  out = AL_FORMAT_71CHN8; break;
                    case Type::S16: out = AL_FORMAT_71CHN16; break;
                    case Type::F32: out = AL_FORMAT_71CHN32; break;
                    default: break;
                    }
                    break;
                }
                return out;
            }*/

            inline RtAudioFormat toRtAudio(Type value)
            {
                RtAudioFormat out = 0;
                switch (value)
                {
                case AV::Audio::Type::S16: out = RTAUDIO_SINT16; break;
                case AV::Audio::Type::S32: out = RTAUDIO_SINT32; break;
                case AV::Audio::Type::F32: out = RTAUDIO_FLOAT32; break;
                case AV::Audio::Type::F64: out = RTAUDIO_FLOAT64; break;
                default: break;
                }
                return out;
            }

            inline void S8ToS16(S8_T value, S16_T & out)
            {
                out = value << 8;
            }

            inline void S8ToS32(S8_T value, S32_T& out)
            {
                out = value << 24;
            }

            inline void S8ToF32(S8_T value, F32_T & out)
            {
                out = value / static_cast<float>(S8Max);
            }

            inline void S8ToF64(S8_T value, F64_T & out)
            {
                out = value / static_cast<double>(S8Max);
            }

            inline void S16ToS8(S16_T value, S8_T & out)
            {
                out = value >> 8;
            }

            inline void S16ToS32(S16_T value, S32_T& out)
            {
                out = value << 16;
            }

            inline void S16ToF32(S16_T value, F32_T & out)
            {
                out = value / static_cast<float>(S16Max);
            }

            inline void S16ToF64(S16_T value, F64_T & out)
            {
                out = value / static_cast<double>(S16Max);
            }

            inline void S32ToS8(S32_T value, S8_T& out)
            {
                out = value >> 24;
            }

            inline void S32ToS16(S32_T value, S16_T& out)
            {
                out = value >> 16;
            }

            inline void S32ToF32(S32_T value, F32_T& out)
            {
                out = value / static_cast<float>(S32Max);
            }

            inline void S32ToF64(S32_T value, F64_T& out)
            {
                out = value / static_cast<double>(S32Max);
            }

            inline void F32ToS8(F32_T value, S8_T & out)
            {
                out = static_cast<S8_T>(Core::Math::clamp(
                    static_cast<int16_t>(value * S8Max),
                    static_cast<int16_t>(S8Min),
                    static_cast<int16_t>(S8Max)));
            }

            inline void F32ToS16(F32_T value, S16_T & out)
            {
                out = static_cast<S16_T>(Core::Math::clamp(
                    static_cast<int32_t>(value * S16Max),
                    static_cast<int32_t>(S16Min),
                    static_cast<int32_t>(S16Max)));
            }

            inline void F32ToS32(F32_T value, S32_T& out)
            {
                out = static_cast<S32_T>(Core::Math::clamp(
                    static_cast<int64_t>(static_cast<int64_t>(value) * S32Max),
                    static_cast<int64_t>(S32Min),
                    static_cast<int64_t>(S32Max)));
            }

            inline void F32ToF64(F32_T value, F64_T& out)
            {
                out = static_cast<double>(value);
            }

            inline void F64ToS8(F64_T value, S8_T & out)
            {
                out = static_cast<S8_T>(Core::Math::clamp(
                    static_cast<int16_t>(value * S8Max),
                    static_cast<int16_t>(S8Min),
                    static_cast<int16_t>(S8Max)));
            }

            inline void F64ToS16(F64_T value, S16_T & out)
            {
                out = static_cast<S16_T>(Core::Math::clamp(
                    static_cast<int32_t>(value * S16Max),
                    static_cast<int32_t>(S16Min),
                    static_cast<int32_t>(S16Max)));
            }

            inline void F64ToS32(F64_T value, S32_T& out)
            {
                out = static_cast<S32_T>(Core::Math::clamp(
                    static_cast<int64_t>(static_cast<int64_t>(value) * S32Max),
                    static_cast<int64_t>(S32Min),
                    static_cast<int64_t>(S32Max)));
            }

            inline void F64ToF32(F64_T value, F32_T& out)
            {
                out = static_cast<float>(value);
            }

        } // namespace Audio
    } // namespace AV
} // namespace djv
