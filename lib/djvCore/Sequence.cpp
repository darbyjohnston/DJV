
//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvCore/Sequence.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Math.h>
#include <djvCore/SequenceUtil.h>

#include <QCoreApplication>

namespace djv
{
    namespace Core
    {
        const QStringList & Sequence::compressLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::Sequence", "Off") <<
                qApp->translate("djv::Core::Sequence", "Sparse") <<
                qApp->translate("djv::Core::Sequence", "Range");
            DJV_ASSERT(data.count() == COMPRESS_COUNT);
            return data;
        }

        Sequence::Sequence()
        {}

        Sequence::Sequence(const FrameList & frames, int pad, const Speed & speed) :
            frames(frames),
            pad(pad),
            speed(speed)
        {}

        Sequence::Sequence(qint64 start, qint64 end, int pad, const Speed & speed) :
            pad(pad),
            speed(speed)
        {
            setFrames(start, end);
        }

        namespace
        {
            // Set the maximum number of frames large enough for a two hour movie.
            qint64 _maxFrames = Sequence::maxFramesDefault();

        } // namespace

        void Sequence::setFrames(qint64 start, qint64 end)
        {
            if (start < end)
            {
                const qint64 size = Math::min<qint64>(end - start + 1, _maxFrames);
                frames.resize(size);
                for (qint64 i = start, j = 0; i <= end && j < size; ++i, ++j)
                {
                    frames[j] = i;
                }
            }
            else
            {
                const qint64 size = Math::min<qint64>(start - end + 1, _maxFrames);
                frames.resize(size);
                for (qint64 i = start, j = 0; i >= end && j < size; --i, ++j)
                {
                    frames[j] = i;
                }
            }
        }

        namespace
        {
            bool compare(qint64 a, qint64 b)
            {
                return a < b;
            }

        } // namespace

        void Sequence::sort()
        {
            qSort(frames.begin(), frames.end(), compare);
        }

        qint64 Sequence::maxFramesDefault()
        {
            return 4 * 60 * 60 * 24;
        }

        qint64 Sequence::maxFrames()
        {
            return _maxFrames;
        }

        void Sequence::setMaxFrames(qint64 size)
        {
            _maxFrames = size;
        }

    } // namespace Core

    _DJV_STRING_OPERATOR_LABEL(Core::Sequence::COMPRESS, Core::Sequence::compressLabels());

    QStringList & operator << (QStringList & out, const Core::Sequence & in)
    {
        return out << Core::SequenceUtil::sequenceToString(in);
    }

    QStringList & operator >> (QStringList & in, Core::Sequence & out) throw (QString)
    {
        QString tmp;
        in >> tmp;
        out = Core::SequenceUtil::stringToSequence(tmp);
        return in;
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::Sequence & in)
    {
        return debug <<
            in <<
            "@" <<
            Core::Speed::speedToFloat(in.speed);
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::Sequence::COMPRESS & in)
    {
        return debug << in;
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::FrameList & in)
    {
        Q_FOREACH(qint64 i, in)
        {
            debug << i;
        }
        return debug;
    }

} // namespace djv
