// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <set>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;
            class Path;

        } // namespace FileSystem
    } // namespace System

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
                void _init(const std::shared_ptr<System::Context>&);
                FileBrowser();

            public:
                ~FileBrowser() override;

                static std::shared_ptr<FileBrowser> create(const std::shared_ptr<System::Context>&);

                void setFileExtensions(const std::set<std::string>&);

                const System::File::Path& getPath() const;

                void setPath(const System::File::Path &);
                void setCallback(const std::function<void(const System::File::Info&)>&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init &) override;

            private:
                std::string _getItemCountLabel(size_t) const;

                void _optionsUpdate();

                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

