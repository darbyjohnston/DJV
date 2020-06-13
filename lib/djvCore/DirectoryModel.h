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
                void setPath(const Path &);

                ///@}

                //! \name Directory Contents
                ///@{

                std::shared_ptr<IListSubject<FileInfo> > observeFileInfo() const;
                std::shared_ptr<IListSubject<std::string> > observeFileNames() const;

                void reload();

                ///@}

                //! \name Navigation
                ///@{

                void cdUp();
                std::shared_ptr<IValueSubject<bool> > observeHasUp() const;

                ///@}

                //! \name History
                ///@{

                void setHistoryMax(size_t);
                void setHistoryIndex(size_t);
                void goBack();
                void goForward();
                std::shared_ptr<IListSubject<Path> > observeHistory() const;
                std::shared_ptr<IValueSubject<size_t> > observeHistoryIndex() const;
                std::shared_ptr<IValueSubject<bool> > observeHasBack() const;
                std::shared_ptr<IValueSubject<bool> > observeHasForward() const;

                ///@}

                //! \name Options
                ///@{

                void setFileExtensions(const std::set<std::string>&);

                std::shared_ptr<IValueSubject<bool> > observeFileSequences() const;
                std::shared_ptr<IValueSubject<bool> > observeShowHidden() const;
                void setFileSequences(bool);
                void setFileSequenceExtensions(const std::set<std::string>&);
                void setShowHidden(bool);

                std::shared_ptr<IValueSubject<DirectoryListSort> > observeSort() const;
                std::shared_ptr<IValueSubject<bool> > observeReverseSort() const;
                std::shared_ptr<IValueSubject<bool> > observeSortDirectoriesFirst() const;
                void setSort(DirectoryListSort);
                void setReverseSort(bool);
                void setSortDirectoriesFirst(bool);

                std::shared_ptr<IValueSubject<std::string> > observeFilter() const;
                void setFilter(const std::string &);
                void clearFilter();

                ///@}

            private:
                void _updatePath();

                DJV_PRIVATE();
            };

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

