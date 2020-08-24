// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <set>

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
        //! This namespace provides file browser functionality.
        namespace FileBrowser
        {
            //! This class provides a file browser widget.
            class FileBrowser : public Widget
            {
                DJV_NON_COPYABLE(FileBrowser);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                FileBrowser();

            public:
                ~FileBrowser() override;

                static std::shared_ptr<FileBrowser> create(const std::shared_ptr<Core::Context>&);

                void setFileExtensions(const std::set<std::string>&);

                const Core::FileSystem::Path& getPath() const;
                void setPath(const Core::FileSystem::Path &);
                void setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init &) override;

            private:
                std::string _getItemCountLabel(size_t) const;

                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

