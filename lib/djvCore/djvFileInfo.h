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

//! \file djvFileInfo.h

#ifndef DJV_FILE_INFO_H
#define DJV_FILE_INFO_H

#include <djvSequence.h>
#include <djvUser.h>

#include <QMetaType>
#include <QSet>
#include <QString>

#include <sys/types.h>

class djvFileInfoUtil;

//! \addtogroup djvCoreFile
//@{

//------------------------------------------------------------------------------
//! \class djvFileInfo
//!
//! This class provides information about files and file sequences. A file
//! sequence is a list of file names that share a common name and have frame
//! numbers. File sequences are used to store animation or movie footage where
//! each frame is an individual file. A file sequence is expressed as a file
//! name with a start and end frame separated with a dash ('-'), for example
//! "render.1-100.exr". Frame increments may be specified with a colon (':'),
//! for example "render.1-100:3.exr" Multiple start and end frames and
//! individual frames are separated with a comma (','), for example
//! "render.1-10,20-30,33,35,37.exr". File sequences may also have frames with
//! padded zeroes, for example "render.0001-0100.exr". Note that file sequences
//! are always sorted in ascending order and frame numbers are always positive.
//!
//! File names are broken into components which are returned with path(),
//! prefix(), number(), and extension().
//!
//! The file type is returned with type(), and information is returned with
//! size(), owner(), and time(). File sequences return information for the
//! entire sequence.
//!
//! Example file name:
//!
//! \code
//! /scratch/render.exr
//! \endcode
//!
//! - Path          = /scratch/
//! - Prefix        = render
//! - Number        =
//! - Frame padding = 0
//! - Extension     = .exr
//!
//! Example file sequence:
//!
//! \code
//! /scratch/render.1-100.exr
//! \endcode
//!
//! - Path          = /scratch/
//! - Prefix        = render.
//! - Number        = 1-100
//! - Frame padding = 0
//! - Extension     = .exr
//!
//! Example file sequence with frame padding:
//!
//! \code
//! /scratch/render.1-100.exr
//! \endcode
//!
//! - Path          = /scratch/
//! - Prefix        = render.
//! - Number        = 0001-0100
//! - Frame padding = 4
//! - Extension     = .exr
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvFileInfo
{
    Q_GADGET
    Q_ENUMS(TYPE)
    Q_FLAGS(PERMISSIONS)
    
public:

    //! Constructor.

    djvFileInfo();

    //! Constructor.

    djvFileInfo(const QString &, bool stat = true);

    //! Get the file name.
    //
    //! \param frame Specify a frame number or -1 for the entire sequence.
    //! \param path Include the path in the file name.

    QString fileName(qint64 frame = -1, bool path = true) const;

    //! Set the file name.
    //
    //! \param stat Get information from the file system.

    void setFileName(const QString &, bool stat = true);

    //! Get the file name without the path.

    inline QString name() const;

    //! Get the path.

    inline const QString & path() const;

    //! Set the path.

    void setPath(const QString &);

    //! Get the base.

    inline const QString & base() const;

    //! Set the base.

    void setBase(const QString &);

    //! Get the number.

    inline const QString & number() const;

    //! Set the number.

    void setNumber(const QString &);

    //! Get the extension.

    inline const QString & extension() const;

    //! Set the extension.

    void setExtension(const QString &);
    
    //! Get whether the file is empty.
    
    inline bool isEmpty() const;
    
    //! Get whether this file exists.
    
    inline bool exists() const;
    
    //! Get whether this is a dot file (name starts with a ".").
    
    inline bool isDotFile() const;

    //! This enumeration provides the file types.

    enum TYPE
    {
        FILE,      //!< Regular file
        SEQUENCE,  //!< File sequence
        DIRECTORY, //!< Directory

        TYPE_COUNT
    };

    //! Get the file type labels.

    static const QStringList & typeLabels();

    //! This enumeration provides the file permissions.

    enum PERMISSIONS
    {
        READ  = 1, //!< Readable
        WRITE = 2, //!< Writable
        EXEC  = 4  //!< Executable
    };

    static const int PERMISSIONS_COUNT = 3;

    //! Get the file permissions labels.

    static const QStringList & permissionsLabels();

    //! Get a file permissions label.

    static const QString & permissionsLabel(int);

    //! Get the type.

    inline TYPE type() const;

    //! Set the type.

    void setType(TYPE);

    //! Get the size.

    inline quint64 size() const;

    //! Set the size.

    void setSize(quint64);

    //! Get the user.

    inline uid_t user() const;

    //! Set the user.

    void setUser(uid_t);

    //! Get the permissions.

    inline int permissions() const;

    //! Set the permissions.

    void setPermissions(int);

    //! Get the time.

    inline time_t time() const;

    //! Set the time.

    void setTime(time_t);

    //! Get information from the file system.

    bool stat(const QString & path = QString());

    //! Get the sequence.

    inline const djvSequence & sequence() const;

    //! Set the sequence.

    void setSequence(const djvSequence &);

    //! Sort the sequence.
    
    void sortSequence();
    
    //! Is the sequence valid?

    inline bool isSequenceValid() const;

    //! Is the sequence a wildcard?

    inline bool isSequenceWildcard() const;

    //! Add a file to the sequence. The file is checked to see whether it is
    //! compatible.

    inline bool addSequence(const djvFileInfo &);

    //! This contains the set of file extensions that can be sequenced.

    static QSet<QString> sequenceExtensions;

    inline bool operator == (const djvFileInfo &) const;
    
    inline bool operator != (const djvFileInfo &) const;
    
    inline operator QString() const;

private:

    void init();

    QString     _path;
    QString     _base;
    QString     _number;
    QString     _extension;

    bool        _exists;
    bool        _dotFile;
    TYPE        _type;
    quint64     _size;
    uid_t       _user;
    int         _permissions;
    time_t      _time;

    djvSequence _sequence;

    friend class djvFileInfoUtil;
};

//------------------------------------------------------------------------------

//! This typedef provides a file information list.

typedef QVector<djvFileInfo> djvFileInfoList;

Q_DECLARE_METATYPE(djvFileInfo)

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvFileInfo::TYPE);
DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvFileInfo);

DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvFileInfo::TYPE);
DJV_DEBUG_OPERATOR(DJV_CORE_EXPORT, djvFileInfo);

//@} // djvCoreFile

#include <djvFileInfoInline.h>

#endif // DJV_FILE_INFO_H

