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

#pragma once

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class Context;

        namespace FileSystem
        {
            class FileInfo;
            class Path;

            //! This class provides a directory model.
            class DirectoryModel : public std::enable_shared_from_this<DirectoryModel>
            {
                DJV_NON_COPYABLE(DirectoryModel);
                void _init(Context *);
                DirectoryModel();

            public:
                ~DirectoryModel();

                static std::shared_ptr<DirectoryModel> create(Context *);
                static std::shared_ptr<DirectoryModel> create(const Path&, Context *);

                std::shared_ptr<IValueSubject<Path> > observePath() const;

                //! Throws:
                //! - std::exception
                void setPath(const Path&);

                void reload();

                std::shared_ptr<IListSubject<FileInfo> > observeFileInfoList() const;
                std::shared_ptr<IListSubject<std::string> > observeFileNames() const;

                std::shared_ptr<IValueSubject<bool> > observeFileSequencesEnabled() const;
                void setFileSequencesEnabled(bool);

                void cdUp();
                std::shared_ptr<IValueSubject<bool> > observeHasUp() const;

                void setHistoryMax(size_t);
                void setHistoryIndex(size_t);
                void goBack();
                void goForward();
                std::shared_ptr<IListSubject<Path> > observeHistory() const;
                std::shared_ptr<IValueSubject<size_t> > observeHistoryIndex() const;
                std::shared_ptr<IValueSubject<bool> > observeHasBack() const;
                std::shared_ptr<IValueSubject<bool> > observeHasForward() const;

            private:
                void _updatePath();

                DJV_PRIVATE();
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

