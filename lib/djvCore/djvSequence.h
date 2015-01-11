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

//! \file djvSequence.h

#ifndef DJV_SEQUENCE_H
#define DJV_SEQUENCE_H

#include <djvSpeed.h>

#include <QMetaType>
#include <QVector>

//! \addtogroup djvCoreMisc
//@{

//! This typedef provides a frame number list.

typedef QVector<qint64> djvFrameList;

//------------------------------------------------------------------------------
//! \class djvSequence
//!
//! This class provides a sequence of frames.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvSequence
{
    Q_GADGET
    Q_ENUMS(COMPRESS)

public:

    //! This enumeration provides the sequence compression.

    enum COMPRESS
    {
        COMPRESS_OFF,     //!< No sequence compression
        COMPRESS_SPARSE,  //!< Sparse sequence compression, for example: 1-3,5
        COMPRESS_RANGE,   //!< Range sequence compression, for example: 1-5

        COMPRESS_COUNT
    };

    //! Get the compression labels.

    static const QStringList & compressLabels();    

    //! Constructor.

    djvSequence();

    //! Constructor.

    explicit djvSequence(const djvFrameList &, int pad = 0, const djvSpeed & = djvSpeed());

    //! Constructor.

    djvSequence(qint64 start, qint64 end, int pad = 0, const djvSpeed & = djvSpeed());

    //! The list of frame numbers.

    djvFrameList frames;

    //! The frame number padding; e.g., 0010.

    int pad;

    //! The speed.

    djvSpeed speed;

    //! Set the list of frame numbers.

    void setFrames(qint64 start, qint64 end);

    //! Get the start frame.

    inline qint64 start() const;

    //! Get the end frame.

    inline qint64 end() const;

    //! Sort the frame numbers in a sequence.

    void sort();
    
    //! Get the maximum number of frames default.

    static qint64 maxFramesDefault();

    //! Get the maximum number of frames a sequence can hold.

    static qint64 maxFrames();

    //! Set the maximum number of frames a sequence can hold.

    static void setMaxFrames(qint64);
};

//------------------------------------------------------------------------------

Q_DECLARE_METATYPE(djvSequence)

inline bool operator == (const djvSequence &, const djvSequence &);

inline bool operator != (const djvSequence &, const djvSequence &);

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvSequence::COMPRESS);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvSequence);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvSequence);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvSequence::COMPRESS);

//@} // djvCoreMisc

#include <djvSequenceInline.h>

#endif // DJV_SEQUENCE_H

