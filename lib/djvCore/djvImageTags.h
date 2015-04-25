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

//! \file djvImageTags.h

#ifndef DJV_IMAGE_TAGS_H
#define DJV_IMAGE_TAGS_H

#include <djvDebug.h>

#include <QMetaType>

struct djvImageTagsPrivate;

//! \addtogroup djvCoreImage
//@{

//------------------------------------------------------------------------------
//! \class djvImageTags
//!
//! This class provides a collection of image tags.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvImageTags
{
    Q_GADGET
    Q_ENUMS(TAGS)
    
public:

    //! Constructor.

    djvImageTags();

    //! Constructor.

    djvImageTags(const djvImageTags &);

    //! Destructor.

    ~djvImageTags();

    //! Add image tags.

    void add(const djvImageTags &);

    //! Add an image tag.

    void add(const QString & key, const QString & value);

    //! Get an image tag.

    QString tag(const QString & key) const;

    //! Get the list of keys.

    QStringList keys() const;

    //! Get the list of values.

    QStringList values() const;

    //! Get the number of tags.

    int count() const;

    //! Get whether the given key is valid.

    bool isValid(const QString & key);

    //! Remove all the tags.

    void clear();

    //! This enumeration provides the standard image tags.

    enum TAGS
    {
        PROJECT,
        CREATOR,
        DESCRIPTION,
        COPYRIGHT,
        TIME,
        UTC_OFFSET,
        KEYCODE,
        TIMECODE,

        TAGS_COUNT
    };

    //! Get the image tag labels.

    static const QStringList & tagLabels();

    djvImageTags & operator = (const djvImageTags &);

    QString & operator [] (const QString & key);

    QString operator [] (const QString & key) const;

private:
    
    djvImageTagsPrivate * _p;
};

//------------------------------------------------------------------------------

Q_DECLARE_METATYPE(djvImageTags)

DJV_COMPARISON_OPERATOR(DJV_CORE_EXPORT, djvImageTags);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvImageTags);

//@} // djvCoreImage

#endif // DJV_IMAGE_TAGS_H

