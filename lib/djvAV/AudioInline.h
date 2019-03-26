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
            constexpr size_t getByteCount(Type value)
            {
                const size_t data[] = { 0, 1, 2, 4, 4 };
                return data[static_cast<size_t>(value)];
            }

            constexpr Type getIntType(size_t value)
            {
                const Type data[] = { Type::None, Type::U8, Type::S16, Type::None, Type::S32 };
                return value < sizeof(data) ? data[value] : Type::None;
            }

            constexpr Type getFloatType(size_t value)
            {
                const Type data[] = { Type::None, Type::None, Type::None, Type::None, Type::F32 };
                return value < sizeof(data) ? data[value] : Type::None;
            }

            inline ALenum getALType(size_t channels, Type type)
            {
                ALenum out = AL_NONE;
                switch (channels)
                {
                case 1:
                    switch (type)
                    {
                    case Type::S16: out = AL_FORMAT_MONO16; break;
                    default: break;
                    }
                    break;
                case 2:
                    switch (type)
                    {
                    case Type::S16: out = AL_FORMAT_STEREO16; break;
                    default: break;
                    }
                    break;
                }
                return out;
            }

            inline void U8ToS16(U8_T value, S16_T & out)
            {
                out = (value << 8) - S16Min;
            }

            inline void U8ToS32(U8_T value, S32_T & out)
            {
                out = (value << 24) - S32Min;
            }

            inline void U8ToF32(U8_T value, F32_T & out)
            {
                out = value / static_cast<float>(U8Max) * 2.f - 1.f;
            }

            inline void S16ToU8(S16_T value, U8_T & out)
            {
                out = (value - S16Min) >> 8;
            }

            inline void S16ToS32(S16_T value, S32_T & out)
            {
                out = value << 16;
            }

            inline void S16ToF32(S16_T value, F32_T & out)
            {
                out = value / static_cast<float>(S16Max);
            }

            inline void S32ToU8(S32_T value, U8_T & out)
            {
                out = (value - S32Min) >> 24;
            }

            inline void S32ToS16(S32_T value, S16_T & out)
            {
                out = value >> 16;
            }

            inline void S32ToF32(S32_T value, F32_T & out)
            {
                out = value / static_cast<float>(S32Max);
            }

            inline void F32ToU8(F32_T value, U8_T & out)
            {
                out = static_cast<U8_T>(Core::Math::clamp(
                    static_cast<int16_t>((value * .5f + .5f) * U8Max),
                    static_cast<int16_t>(U8Min),
                    static_cast<int16_t>(U8Max)));
            }

            inline void F32ToS16(F32_T value, S16_T & out)
            {
                out = static_cast<S16_T>(Core::Math::clamp(
                    static_cast<int32_t>(value * S16Max),
                    static_cast<int32_t>(S16Min),
                    static_cast<int32_t>(S16Max)));
            }

            inline void F32ToS32(F32_T value, S32_T & out)
            {
                out = static_cast<S32_T>(Core::Math::clamp(
                    static_cast<int64_t>(value * S32Max),
                    static_cast<int64_t>(S32Min),
                    static_cast<int64_t>(S32Max)));
            }

        } // namespace Audio
    } // namespace AV
} // namespace djv
