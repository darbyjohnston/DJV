//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <djvAV/FFmpeg.h>

#include <djvCore/Assert.h>
#include <djvCore/DebugLog.h>
#include <djvCore/Error.h>
#include <djvCore/FileIOUtil.h>
#include <djvCore/Math.h>
#include <djvCore/StringUtil.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        FFmpeg::Dictionary::Dictionary() :
            _p(0)
        {}

        FFmpeg::Dictionary::~Dictionary()
        {
            av_dict_free(&_p);
        }

        QMap<QString, QString> FFmpeg::Dictionary::map() const
        {
            QMap<QString, QString> out;
            AVDictionaryEntry * entry = 0;
            while ((entry = av_dict_get(_p, "", entry, AV_DICT_IGNORE_SUFFIX)))
            {
                out.insert(entry->key, entry->value);
            }
            return out;
        }

        AVDictionary ** FFmpeg::Dictionary::operator () ()
        {
            return &_p;
        }

        const AVDictionary * const * FFmpeg::Dictionary::operator () () const
        {
            return &_p;
        }

        FFmpeg::Packet::Packet()
        {
            av_init_packet(&_p);
        }

        FFmpeg::Packet::~Packet()
        {
            av_packet_unref(&_p);
        }

        AVPacket & FFmpeg::Packet::operator () ()
        {
            return _p;
        }

        const AVPacket & FFmpeg::Packet::operator () () const
        {
            return _p;
        }

        FFmpeg::Options::Options() :
            format(MPEG4),
            quality(HIGH)
        {}

        const QString FFmpeg::staticName = "FFmpeg";

        const QStringList & FFmpeg::formatLabels()
        {
            static const QStringList data = QStringList() <<
                //qApp->translate("djv::AV::FFmpeg", "H264") <<
                qApp->translate("djv::AV::FFmpeg", "MPEG4") <<
                qApp->translate("djv::AV::FFmpeg", "ProRes") <<
                qApp->translate("djv::AV::FFmpeg", "MJPEG");
            DJV_ASSERT(data.count() == FORMAT_COUNT);
            return data;
        }

        const QStringList & FFmpeg::qualityLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::FFmpeg", "Low") <<
                qApp->translate("djv::AV::FFmpeg", "Medium") <<
                qApp->translate("djv::AV::FFmpeg", "High");
            DJV_ASSERT(data.count() == QUALITY_COUNT);
            return data;
        }

        AVRational FFmpeg::timeBaseQ()
        {
            AVRational r;
            r.num = 1;
            r.den = AV_TIME_BASE;
            return r;
        }

        QString FFmpeg::toString(int r)
        {
            char buf[Core::StringUtil::cStringLength];
            av_strerror(r, buf, Core::StringUtil::cStringLength);
            return QString(buf);
        }

        const QStringList & FFmpeg::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::FFmpeg", "Format") <<
                qApp->translate("djv::AV::FFmpeg", "Quality");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    _DJV_STRING_OPERATOR_LABEL(AV::FFmpeg::FORMAT, AV::FFmpeg::formatLabels())
    _DJV_STRING_OPERATOR_LABEL(AV::FFmpeg::QUALITY, AV::FFmpeg::qualityLabels())

} // namespace djv
