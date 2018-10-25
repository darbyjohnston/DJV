
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

#include <djvCore/Sequence.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Math.h>

#include <QCoreApplication>

namespace djv
{
    namespace Core
    {
        namespace
        {
            Sequence::FORMAT _format = Sequence::FORMAT_SPARSE;
            bool _autoEnabled = true;
            qint64 _maxSize = 50000;
            bool _negativeEnabled = false;

        } // namespace

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

        void Sequence::setFrames(qint64 start, qint64 end)
        {
            if (start < end)
            {
                const qint64 size = Math::min<qint64>(end - start + 1, _maxSize);
                frames.resize(size);
                for (qint64 i = start, j = 0; i <= end && j < size; ++i, ++j)
                {
                    frames[j] = i;
                }
            }
            else
            {
                const qint64 size = Math::min<qint64>(start - end + 1, _maxSize);
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

        qint64 Sequence::findClosest(qint64 frame, const FrameList & frames)
        {
            const int count = frames.count();
            if (!count)
                return -1;
            qint64 out = 0;
            qint64 min = 0;
            for (int i = 0; i < count; ++i)
            {
                const qint64 tmp = Math::abs(frame - frames[i]);
                if (tmp < min || 0 == i)
                {
                    out = static_cast<qint64>(i);
                    min = tmp;
                }
            }
            return out;
        }

        const QStringList & Sequence::formatLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::Sequence", "Off") <<
                qApp->translate("djv::Core::Sequence", "Sparse") <<
                qApp->translate("djv::Core::Sequence", "Range");
            DJV_ASSERT(data.count() == FORMAT_COUNT);
            return data;
        }

        Sequence::FORMAT Sequence::formatDefault()
        {
            return FORMAT_SPARSE;
        }

        Sequence::FORMAT Sequence::format()
        {
            return _format;
        }

        void Sequence::setFormat(FORMAT value)
        {
            _format = value;
        }

        QString Sequence::frameToString(qint64 frame, int pad)
        {
            const bool negative = frame < 0;
            const quint64 abs = negative ? -frame : frame;
            char c[StringUtil::cStringLength] = "";
            const int length = StringUtil::intToString<qint64>(abs, c);
            QString p;
            if (negative)
            {
                p.append('-');
            }
            for (int i = 0; i < pad - length; ++i)
            {
                p.append('0');
            }
            for (int i = 0; i < length; ++i)
            {
                p.append(c[i]);
            }
            return p;
        }

        namespace
        {
            enum INC
            {
                BREAK,
                ASCEND,
                DESCEND
            };

            INC inc(qint64 a, qint64 b)
            {
                if (a + 1 == b)
                {
                    return ASCEND;
                }
                else if (a - 1 == b)
                {
                    return DESCEND;
                }
                return BREAK;
            }

        } // namespace

        QString Sequence::sequenceToString(const Sequence & seq)
        {
            //DJV_DEBUG("Sequence::sequenceToString");
            //DJV_DEBUG_PRINT("frames = " << in.frames);

            QStringList out;
            FrameList frames = seq.frames;
            const int    count = frames.count();
            const int    pad = seq.pad;

            // Add the list end marker.
            static const qint64 marker = std::numeric_limits<qint64>::min();
            frames += marker;

            // Sequence.
            qint64 tmp = frames[0];
            INC _inc = BREAK;
            for (int i = 0; i < count; ++i)
            {
                const INC inc_ = inc(frames[i], frames[i + 1]);
                //DJV_DEBUG_PRINT(frames[i] << " = " << inc_);
                if ((inc_ != _inc && _inc != BREAK) || BREAK == inc_)
                {
                    if (tmp != frames[i])
                    {
                        out += frameToString(tmp, pad) +
                            "-" +
                            frameToString(frames[i], pad);
                    }
                    else
                    {
                        out += frameToString(frames[i], pad);
                    }
                    tmp = frames[i + 1];
                    _inc = inc_;
                }
            }
            //DJV_DEBUG_PRINT("out = " << out);
            return out.join(",");
        }

        Sequence Sequence::stringToSequence(const QString & seq)
        {
            //DJV_DEBUG("Sequence::stringToSequence");
            //DJV_DEBUG_PRINT("seq = " << seq);

            Sequence out;
            int pad = 0;
            Q_FOREACH(const QString & s, seq.split(',', QString::SkipEmptyParts))
            {
                const int count = s.count();
                if (count)
                {
                    QString a, b;
                    int j = 0;
                    if ('-' == s[j])
                    {
                        a.append(s[j]);
                        ++j;
                    }
                    while (j < count && s[j].isNumber())
                    {
                        a.append(s[j]);
                        ++j;
                    }
                    if (j < count - 1 && '-' == s[j] && '-' == s[j + 1])
                    {
                        ++j;
                    }
                    else if (j < count && '-' == s[j])
                    {
                        ++j;
                    }
                    while (j < count)
                    {
                        b.append(s[j]);
                        ++j;
                    }
                    //DJV_DEBUG_PRINT("a = " << a);
                    //DJV_DEBUG_PRINT("b = " << b);

                    int          _pad = 0;
                    const qint64 start = stringToFrame(a, &_pad);
                    const qint64 end = b.count() ? stringToFrame(b) : start;
                    if (start < end)
                    {
                        const qint64 size = end - start + 1;

                        for (qint64 j = start, k = 0; j <= end && k < size; ++j, ++k)
                        {
                            out.frames += j;
                        }
                    }
                    else
                    {
                        const qint64 size = start - end + 1;
                        for (qint64 j = start, k = 0; j >= end && k < size; --j, ++k)
                        {
                            out.frames += j;
                        }
                    }
                    pad = Math::max(_pad, pad);
                }
            }
            out.pad = pad;
            //DJV_DEBUG_PRINT("out = " << out);
            //DJV_DEBUG_PRINT("pad = " << out.pad);
            return out;
        }

        bool Sequence::autoEnabledDefault()
        {
            return true;
        }

        bool Sequence::isAutoEnabled()
        {
            return _autoEnabled;
        }

        void Sequence::setAutoEnabled(bool value)
        {
            _autoEnabled = value;
        }

        qint64 Sequence::maxSizeDefault()
        {
            return 50000;
        }

        qint64 Sequence::maxSize()
        {
            return _maxSize;
        }

        void Sequence::setMaxSize(qint64 size)
        {
            _maxSize = size;
        }

        bool Sequence::negativeEnabledDefault()
        {
            return false;
        }

        bool Sequence::isNegativeEnabled()
        {
            return _negativeEnabled;
        }

        void Sequence::setNegativeEnabled(bool value)
        {
            _negativeEnabled = value;
        }

    } // namespace Core
    
    _DJV_STRING_OPERATOR_LABEL(Core::Sequence::FORMAT, Core::Sequence::formatLabels());

    QStringList & operator << (QStringList & out, const Core::Sequence & in)
    {
        return out << Core::Sequence::sequenceToString(in);
    }

    QStringList & operator >> (QStringList & in, Core::Sequence & out)
    {
        QString tmp;
        in >> tmp;
        out = Core::Sequence::stringToSequence(tmp);
        return in;
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::FrameList & in)
    {
        Q_FOREACH(qint64 i, in)
        {
            debug << i;
        }
        return debug;
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::Sequence & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp << "@" << Core::Speed::speedToFloat(in.speed);
    }

    Core::Debug & operator << (Core::Debug & debug, const Core::Sequence::FORMAT & in)
    {
        QStringList tmp;
        tmp << in;
        return debug << tmp;
    }

} // namespace djv
