// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;
            class Path;
    
        } // namespace FileSystem
    } // namespace Core

    namespace UI
    {
        namespace FileBrowser
        {
            //! This class provides a file browser dialog.
            class Dialog : public IDialog
            {
                DJV_NON_COPYABLE(Dialog);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Dialog();

            public:
                ~Dialog() override;

                static std::shared_ptr<Dialog> create(const std::shared_ptr<Core::Context>&);

                void setFileExtensions(const std::set<std::string>&);

                const Core::FileSystem::Path& getPath() const;
                void setPath(const Core::FileSystem::Path&);
                void setCallback(const std::function<void(const Core::FileSystem::FileInfo&)>&);

            protected:
                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

