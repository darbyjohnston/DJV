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

#include <AV/Audio.h>

#include <QCoreApplication>

#include <limits>

namespace djv
{
    namespace AV
    {
        const Audio::U8_T   Audio::u8Min = std::numeric_limits<uint8_t>::min();
        const Audio::U8_T   Audio::u8Max = std::numeric_limits<uint8_t>::max();
        const Audio::S16_T Audio::s16Min = std::numeric_limits<int16_t>::min();
        const Audio::S16_T Audio::s16Max = std::numeric_limits<int16_t>::max();
        const Audio::S32_T Audio::s32Min = std::numeric_limits<int32_t>::min();
        const Audio::S32_T Audio::s32Max = std::numeric_limits<int32_t>::max();
        const Audio::F32_T Audio::f32Min = -1.f;
        const Audio::F32_T Audio::f32Max =  1.f;

        const QString & Audio::getLabel(Type value)
        {
            static const std::vector<QString> data =
            {
                qApp->translate("djv::AV::Audio", "None"),
                qApp->translate("djv::AV::Audio", "U8"),
                qApp->translate("djv::AV::Audio", "S16"),
                qApp->translate("djv::AV::Audio", "S32"),
                qApp->translate("djv::AV::Audio", "F32")
            };
            DJV_ASSERT(static_cast<size_t>(Audio::Type::Count) == data.size());
            return data[static_cast<size_t>(value)];
        }

        QString Audio::getALErrorString(ALenum value)
        {
            QString out = qApp->translate("djv::AV::Audio", "Unknown");
            switch (value)
            {
            case AL_OUT_OF_MEMORY: out = qApp->translate("djv::AV::Audio", "Out of memory"); break;
            case AL_INVALID_VALUE: out = qApp->translate("djv::AV::Audio", "Invalid value"); break;
            case AL_INVALID_ENUM:  out = qApp->translate("djv::AV::Audio", "Invalid enum");  break;
            default: break;
            }
            return out;
        }

    } // namespace AV
} // namespace djv

