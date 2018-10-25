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

#pragma once

#include <djvCore/Speed.h>

#include <QMetaType>
#include <QVector>

namespace djv
{
    namespace Core
    {
        //! This typedef provides a frame number list.
        typedef QVector<qint64> FrameList;

        //! This class provides a sequence of frames.
        class Sequence
        {
            Q_GADGET

        public:
            Sequence();
            explicit Sequence(const FrameList &, int pad = 0, const Speed & = Speed());
            Sequence(qint64 start, qint64 end, int pad = 0, const Speed & = Speed());

            FrameList frames;
            int pad = 0;
            Speed speed;

            //! Set the list of frame numbers.
            void setFrames(qint64 start, qint64 end);

            //! Get the start frame.
            inline qint64 start() const;

            //! Get the end frame.
            inline qint64 end() const;

            //! Sort the frame numbers in a sequence.
            void sort();

            //! Find the closest frame in a sequence.
            static qint64 findClosest(qint64, const FrameList &);

            //! This enumeration provides options for how a sequence is
            //! formatted as a string.
            enum FORMAT
            {
                FORMAT_OFF,     //!< No formatting
                FORMAT_SPARSE,  //!< Sparse formatting, for example: 1-3,5
                FORMAT_RANGE,   //!< Range formatting, for example: 1-5

                FORMAT_COUNT
            };
            Q_ENUM(FORMAT);

            //! Get the formatting labels.
            static const QStringList & formatLabels();

            //! Get the default sequence formatting.
            static FORMAT formatDefault();

            //! Get the sequence formatting.
            static FORMAT format();

            //! Set the sequence formatting.
            static void setFormat(FORMAT);

            //! Convert a frame to a string.
            static QString frameToString(qint64, int pad = 0);

            //! Convert a string to a frame.
            static inline qint64 stringToFrame(const QString &, int * pad = 0);

            //! Convert a sequence to a string.
            static QString sequenceToString(const Sequence &);

            //! Convert a string to a sequence.
            static Sequence stringToSequence(const QString &);

            //! Get the auto sequencing default.
            static bool autoEnabledDefault();

            //! Get whether auto sequencing is enabled.
            static bool isAutoEnabled();

            //! Set whether auto sequencing is enabled.
            static void setAutoEnabled(bool);

            //! Get the default maximum sequence size.
            static qint64 maxSizeDefault();

            //! Get the maximum sequence size.
            static qint64 maxSize();

            //! Set the maximum sequence size.
            static void setMaxSize(qint64);

            //! Get the negative numbers default.
            static bool negativeEnabledDefault();

            //! Get whether negative numbers are enabled.
            static bool isNegativeEnabled();

            //! Set whether negative numbers are enabled.
            static void setNegativeEnabled(bool);
        };

    } // namespace Core

    inline bool operator == (const Core::Sequence &, const Core::Sequence &);
    inline bool operator != (const Core::Sequence &, const Core::Sequence &);

    DJV_STRING_OPERATOR(Core::Sequence);
    DJV_STRING_OPERATOR(Core::Sequence::FORMAT);
    DJV_DEBUG_OPERATOR(Core::FrameList);
    DJV_DEBUG_OPERATOR(Core::Sequence);
    DJV_DEBUG_OPERATOR(Core::Sequence::FORMAT);

} // namespace djv

Q_DECLARE_METATYPE(djv::Core::Sequence)
Q_DECLARE_METATYPE(djv::Core::Sequence::FORMAT)

#include <djvCore/SequenceInline.h>

