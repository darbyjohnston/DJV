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

#include <AV/FFmpeg.h>

#include <Core/StringUtil.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        AVRational FFmpeg::timeBaseQ()
        {
            AVRational r;
            r.num = 1;
            r.den = AV_TIME_BASE;
            return r;
        }

        Audio::Type FFmpeg::fromFFmpeg(AVSampleFormat value)
        {
            Audio::Type out = Audio::Type::None;
            switch (value)
            {
            case AV_SAMPLE_FMT_U8:   out = Audio::Type::U8;  break;
            case AV_SAMPLE_FMT_S16:  out = Audio::Type::S16; break;
            case AV_SAMPLE_FMT_S32:  out = Audio::Type::S32; break;
            case AV_SAMPLE_FMT_FLT:  out = Audio::Type::F32; break;
            case AV_SAMPLE_FMT_U8P:  out = Audio::Type::U8;  break;
            case AV_SAMPLE_FMT_S16P: out = Audio::Type::S16; break;
            case AV_SAMPLE_FMT_S32P: out = Audio::Type::S32; break;
            case AV_SAMPLE_FMT_FLTP: out = Audio::Type::F32; break;
            default: break;
            }
            return out;
        }

        QString FFmpeg::toString(AVSampleFormat value)
        {
            static const std::map<AVSampleFormat, QString> data =
            {
                { AV_SAMPLE_FMT_NONE, qApp->translate("djv::AV::FFmpeg", "None") },
                { AV_SAMPLE_FMT_U8, qApp->translate("djv::AV::FFmpeg", "U8") },
                { AV_SAMPLE_FMT_S16, qApp->translate("djv::AV::FFmpeg", "S16") },
                { AV_SAMPLE_FMT_S32, qApp->translate("djv::AV::FFmpeg", "S32") },
                { AV_SAMPLE_FMT_FLT, qApp->translate("djv::AV::FFmpeg", "Float") },
                { AV_SAMPLE_FMT_DBL, qApp->translate("djv::AV::FFmpeg", "Double") },
                { AV_SAMPLE_FMT_U8P, qApp->translate("djv::AV::FFmpeg", "U8 planar") },
                { AV_SAMPLE_FMT_S16P, qApp->translate("djv::AV::FFmpeg", "S16 planar") },
                { AV_SAMPLE_FMT_S32P, qApp->translate("djv::AV::FFmpeg", "S32 planar") },
                { AV_SAMPLE_FMT_FLTP, qApp->translate("djv::AV::FFmpeg", "Float planar") },
                { AV_SAMPLE_FMT_DBLP, qApp->translate("djv::AV::FFmpeg", "Double planar") }
            };
            static const QString unknown = qApp->translate("djv::AV::FFmpeg", "Unknown");
            const auto i = data.find(value);
            return i != data.end() ? i->second : unknown;
        }

        QString FFmpeg::getErrorString(int r)
        {
            char buf[Core::StringUtil::cStringLength];
            av_strerror(r, buf, Core::StringUtil::cStringLength);
            return QString(buf);
        }

    } // namespace AV
} // namespace djv

