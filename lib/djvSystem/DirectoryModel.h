// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/FileInfo.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace System
    {
        class Context;

        namespace File
        {
            //! Directory model.
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

                std::shared_ptr<Core::Observer::IValueSubject<Path> > observePath() const;

                //! Throws:
                //! - Error
                void setPath(const Path&);

                ///@}

                //! \name Contents
                ///@{

                std::shared_ptr<Core::Observer::IListSubject<File::Info> > observeInfo() const;
                std::shared_ptr<Core::Observer::IListSubject<std::string> > observeFileNames() const;

                void reload();

                ///@}

                //! \name Navigation
                ///@{

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeHasUp() const;

                void cdUp();

                ///@}

                //! \name History
                ///@{

                std::shared_ptr<Core::Observer::IListSubject<Path> > observeHistory() const;
                std::shared_ptr<Core::Observer::IValueSubject<size_t> > observeHistoryIndex() const;
                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeHasBack() const;
                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeHasForward() const;

                void setHistoryMax(size_t);
                void setHistoryIndex(size_t);
                void goBack();
                void goForward();

                ///@}

                //! \name Options
                ///@{

                std::shared_ptr<Core::Observer::IValueSubject<DirectoryListOptions> > observeOptions() const;

                void setOptions(const DirectoryListOptions&);

                ///@}

            private:
                void _pathUpdate();

                DJV_PRIVATE();
            };

        } // namespace File
    } // namespace System
} // namespace djv

