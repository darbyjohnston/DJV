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

#include <djvCore/FileInfo.h>

#include <QMetaType>
#include <QStringList>

namespace djv
{
    namespace Core
    {
        //! \class FileInfoUtil
        //!
        //! This class provides file information utilities.
        class FileInfoUtil
        {
            Q_GADGET
            Q_FLAGS(FILTER)
            Q_ENUMS(SORT)

        public:
            virtual ~FileInfoUtil() = 0;

            //! Split a file name into pieces.
            static void split(
                const QString &,
                QString & path,
                QString & base,
                QString & number,
                QString & extension);

            //! Check if a file exists.
            static bool exists(const FileInfo &);

            //! Get a file list from a directory.
            static FileInfoList list(
                const QString &    path,
                Sequence::COMPRESS sequence = Sequence::COMPRESS_RANGE);

            //! Find a match for a sequence wildcard. If nothing is found the input is
            //! returned.
            //!
            //! Example wildcard: render.#.exr
            static const FileInfo & sequenceWildcardMatch(
                const FileInfo &,
                const FileInfoList &);

            //! Compress a list of files into a sequence.
            static void compressSequence(
                FileInfoList &,
                Sequence::COMPRESS = Sequence::COMPRESS_RANGE);

            //! Expand a sequence into a list of file names.
            static QStringList expandSequence(const FileInfo &);

            //! This enumeration provides file filters.
            enum FILTER
            {
                FILTER_NONE = 0,
                FILTER_FILES = 1, //!< Filter files
                FILTER_DIRECTORIES = 2, //!< Filter directories
                FILTER_HIDDEN = 4  //!< Filter hidden items
            };

            //! Filter a list of files.
            static void filter(
                FileInfoList &,
                int                 filter,
                const QString &     filterText = QString(),
                const QStringList & glob = QStringList());

            //! This enumeration provides file sorting.
            enum SORT
            {
                SORT_NAME,        //!< Sort by name
                SORT_TYPE,        //!< Sort by file type
                SORT_SIZE,        //!< Sort by file size
                SORT_USER,        //!< Sort by user name
                SORT_PERMISSIONS, //!< Sort by file permissions
                SORT_TIME,        //!< Sort by the last modified time

                SORT_COUNT
            };

            //! Get the file sorting labels.
            static const QStringList & sortLabels();

            //! Sort a list of files.
            static void sort(
                FileInfoList &,
                SORT,
                bool reverse = false);

            //! Sort the list so directories are first.
            static void sortDirsFirst(FileInfoList &);

            //! The maximum number of recent files.
            static const int recentMax = 10;

            //! Update a recent files list.
            static void recent(
                const FileInfo & fileInfo,
                FileInfoList &   list,
                int              max = recentMax);

            //! Update a recent files list.
            static void recent(
                const QString & fileName,
                QStringList &   list,
                int             max = recentMax);

            //! Fix a path.
            static QString fixPath(const QString &);

            //! Path list separators.
            static QList<QChar> listSeparators;

            //! Path list separator.
#if defined(DJV_WINDOWS)
            static const QChar & listSeparator;
#else // DJV_WINDOWS
            static const QChar & listSeparator;
#endif // DJV_WINDOWS

            //! Dot directory.
            static const QString dot;

            //! Dot dot directory.
            static const QString dotDot;

            //! Parse a file name.
            static FileInfo parse(
                const QString &    fileName,
                Sequence::COMPRESS sequence,
                bool               autoSequence = false);
        };

    } // namespace Core

    DJV_STRING_OPERATOR(Core::FileInfoUtil::SORT);

} // namespace djv
