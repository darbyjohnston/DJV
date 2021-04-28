// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAudio/Type.h>

#include <algorithm>
#include <array>

namespace djv
{
    namespace Audio
    {
        uint8_t getByteCount(Type value)
        {
            const std::array<uint8_t, static_cast<size_t>(Type::Count)> data =
            {
                0,
                1,
                2,
                4,
                4,
                8
            };
            return data[static_cast<size_t>(value)];
        }

        Type getIntType(uint8_t value)
        {
            const std::array<Type, static_cast<size_t>(Type::Count)> data =
            {
                Type::None,
                Type::S8,
                Type::S16,
                Type::S32,
                Type::None,
                Type::None
            };
            return value < data.size() ? data[value] : Type::None;
        }

        Type getFloatType(uint8_t value)
        {
            const std::array<Type, static_cast<size_t>(Type::Count)> data =
            {
                Type::None,
                Type::None,
                Type::None,
                Type::None,
                Type::F32,
                Type::F64
            };
            return value < data.size() ? data[value] : Type::None;
        }

        /*ALenum getALType(uint8_t channels, Type type)
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

        /*std::string getALErrorString(ALenum value)
        {
            std::string out = DJV_TEXT("error_unknown");
            switch (value)
            {
            case AL_OUT_OF_MEMORY: out = DJV_TEXT("error_al_out_of_memory"); break;
            case AL_INVALID_VALUE: out = DJV_TEXT("error_al_invalid_value"); break;
            case AL_INVALID_ENUM:  out = DJV_TEXT("error_al_invalid_enum");  break;
            default: break;
            }
            return out;
        }*/

        RtAudioFormat toRtAudio(Type value) noexcept
        {
            RtAudioFormat out = 0;
            switch (value)
            {
            case Audio::Type::S16: out = RTAUDIO_SINT16; break;
            case Audio::Type::S32: out = RTAUDIO_SINT32; break;
            case Audio::Type::F32: out = RTAUDIO_FLOAT32; break;
            case Audio::Type::F64: out = RTAUDIO_FLOAT64; break;
            default: break;
            }
            return out;
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(Type);

    } // namespace Audio

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Audio,
        Type,
        DJV_TEXT("audio_type_none"),
        DJV_TEXT("audio_type_u8"),
        DJV_TEXT("audio_type_s16"),
        DJV_TEXT("audio_type_s32"),
        DJV_TEXT("audio_type_f32"),
        DJV_TEXT("audio_type_f64"));

} // namespace djv

