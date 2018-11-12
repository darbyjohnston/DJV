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

#include <djvCore/Math.h>

namespace djv
{
    namespace AV
    {
        inline int Audio::byteCount(TYPE value)
        {
            static const int data[] = { 0, 1, 2, 4, 4 };
            return data[value];
        }

        inline Audio::TYPE Audio::intType(size_t value)
        {
            static const TYPE data[] = { TYPE_NONE, U8, S16, TYPE_NONE, S32 };
            return value < sizeof(data) ? data[value] : TYPE_NONE;
        }

        inline Audio::TYPE Audio::floatType(size_t value)
        {
            static const TYPE data[] = { TYPE_NONE, TYPE_NONE, TYPE_NONE, TYPE_NONE, F32 };
            return value < sizeof(data) ? data[value] : TYPE_NONE;
        }

        inline ALenum Audio::toAL(size_t channels, TYPE type)
        {
            ALenum out = AL_NONE;
            switch (channels)
            {
            case 1:
                switch (type)
                {
                case S16: out = AL_FORMAT_MONO16; break;
                default: break;
                }
                break;
            case 2:
                switch (type)
                {
                case S16: out = AL_FORMAT_STEREO16; break;
                default: break;
                }
                break;
            }
            return out;
        }

        inline void Audio::convert(U8_T value, S16_T & out)
        {
            out =  (value << 8) - std::numeric_limits<int16_t>::min();
        }

        inline void Audio::convert(U8_T value, Audio::S32_T & out)
        {
            out = (value << 24) - std::numeric_limits<int32_t>::min();
        }

        inline void Audio::convert(U8_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(std::numeric_limits<uint8_t>::max()) * 2.f - 1.f;
        }

        inline void Audio::convert(S16_T value, Audio::U8_T & out)
        {
            out = (value - std::numeric_limits<int16_t>::min()) >> 8;
        }

        inline void Audio::convert(S16_T value, Audio::S32_T & out)
        {
            out = value << 16;
        }

        inline void Audio::convert(S16_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(std::numeric_limits<int16_t>::max());
        }

        inline void Audio::convert(S32_T value, Audio::U8_T & out)
        {
            out = (value - std::numeric_limits<int32_t>::min()) >> 24;
        }

        inline void Audio::convert(S32_T value, Audio::S16_T & out)
        {
            out = value >> 16;
        }

        inline void Audio::convert(S32_T value, Audio::F32_T & out)
        {
            out = value / static_cast<float>(std::numeric_limits<int32_t>::max());
        }

        inline void Audio::convert(F32_T value, Audio::U8_T & out)
        {
            out = Core::Math::clamp(
                static_cast<int16_t>((value * .5f + .5f) * std::numeric_limits<uint8_t>::max()),
                static_cast<int16_t>(u8Min),
                static_cast<int16_t>(u8Max));
        }

        inline void Audio::convert(F32_T value, Audio::S16_T & out)
        {
            out = Core::Math::clamp(
                static_cast<int32_t>(value * std::numeric_limits<int16_t>::max()),
                static_cast<int32_t>(s16Min),
                static_cast<int32_t>(s16Max));
        }

        inline void Audio::convert(F32_T value, Audio::S32_T & out)
        {
            out = Core::Math::clamp(
                static_cast<int64_t>(value * std::numeric_limits<int32_t>::max()),
                static_cast<int64_t>(s32Min),
                static_cast<int64_t>(s32Max));
        }

    } // namespace AV
} // namespace djv
