// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/FileInfo.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class Context;

        namespace FileSystem
        {
            //! This class provides a directory model.
            class DirectoryModel : public std::enable_shared_from_this<DirectoryModel>
            {
                DJV_NON_COPYABLE(DirectoryModel);
                void _init(const std::shared_ptr<Context>&);
                DirectoryModel();

            public:
                ~DirectoryModel();

                static std::shared_ptr<DirectoryModel> create(const std::shared_ptr<Context>&);

                //! \name Path
                ///@{

                std::shared_ptr<IValueSubject<Path> > observePath() const;

                //! Throws:
                //! - Error
                void setPath(const Path&);

                ///@}

                //! \name Directory Contents
                ///@{

                std::shared_ptr<IListSubject<FileInfo> > observeFileInfo() const;
                std::shared_ptr<IListSubject<std::string> > observeFileNames() const;

                void reload();

                ///@}

                //! \name Navigation
                ///@{

                std::shared_ptr<IValueSubject<bool> > observeHasUp() const;

                void cdUp();

                ///@}

                //! \name History
                ///@{

                std::shared_ptr<IListSubject<Path> > observeHistory() const;
                std::shared_ptr<IValueSubject<size_t> > observeHistoryIndex() const;
                std::shared_ptr<IValueSubject<bool> > observeHasBack() const;
                std::shared_ptr<IValueSubject<bool> > observeHasForward() const;

                void setHistoryMax(size_t);
                void setHistoryIndex(size_t);
                void goBack();
                void goForward();

                ///@}

                //! \name Options
                ///@{

                std::shared_ptr<IValueSubject<DirectoryListOptions> > observeOptions() const;

                void setOptions(const DirectoryListOptions&);

                ///@}

            private:
                void _pathUpdate();

                DJV_PRIVATE();
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

