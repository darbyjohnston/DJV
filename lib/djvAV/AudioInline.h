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

#include <djvCore/MathUtil.h>

namespace djv
{
    namespace AV
    {
        inline size_t Audio::getByteCount(Type value)
        {
            static const size_t data[] = { 0, 1, 2, 4, 4 };
            return data[static_cast<size_t>(value)];
        }

        inline Audio::Type Audio::getIntType(size_t value)
        {
            static const Type data[] = { Type::None, Type::U8, Type::S16, Type::None, Type::S32 };
            return value < sizeof(data) ? data[value] : Type::None;
        }

        inline Audio::Type Audio::getFloatType(size_t value)
        {
            static const Type data[] = { Type::None, Type::None, Type::None, Type::None, Type::F32 };
            return value < sizeof(data) ? data[value] : Type::None;
        }

        inline ALenum Audio::getALType(size_t channels, Type type)
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

        inline void Audio::convert(U8_T value, S16_T & out)
        {
            out = (value << 8) - s16Min;
        }

        inline void Audio::convert(U8_T value, Audio::S32_T & out)
        {
            out = (value << 24) - s32Min;
        }

        inline void Audio::convert(U8_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(u8Max) * 2.f - 1.f;
        }

        inline void Audio::convert(S16_T value, Audio::U8_T & out)
        {
            out = (value - s16Min) >> 8;
        }

        inline void Audio::convert(S16_T value, Audio::S32_T & out)
        {
            out = value << 16;
        }

        inline void Audio::convert(S16_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(s16Max);
        }

        inline void Audio::convert(S32_T value, Audio::U8_T & out)
        {
            out = (value - s32Min) >> 24;
        }

        inline void Audio::convert(S32_T value, Audio::S16_T & out)
        {
            out = value >> 16;
        }

        inline void Audio::convert(S32_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(s32Max);
        }

        inline void Audio::convert(F32_T value, Audio::U8_T & out)
        {
            out = Core::MathUtil::clamp(
                static_cast<int16_t>((value * .5f + .5f) * u8Max),
                static_cast<int16_t>(u8Min),
                static_cast<int16_t>(u8Max));
        }

        inline void Audio::convert(F32_T value, Audio::S16_T & out)
        {
            out = Core::MathUtil::clamp(
                static_cast<int32_t>(value * s16Max),
                static_cast<int32_t>(s16Min),
                static_cast<int32_t>(s16Max));
        }

        inline void Audio::convert(F32_T value, Audio::S32_T & out)
        {
            out = Core::MathUtil::clamp(
                static_cast<int64_t>(value * s32Max),
                static_cast<int64_t>(s32Min),
                static_cast<int64_t>(s32Max));
        }

    } // namespace AV
} // namespace djv
