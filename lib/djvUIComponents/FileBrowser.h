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

    namespace UIComponents
    {
        //! File browser.
        namespace FileBrowser
        {
            //! File browser widget.
            class FileBrowser : public UI::Widget
            {
                DJV_NON_COPYABLE(FileBrowser);

            protected:
                void _init(UI::SelectionType, const std::shared_ptr<System::Context>&);
                FileBrowser();

            public:
                ~FileBrowser() override;

                static std::shared_ptr<FileBrowser> create(UI::SelectionType, const std::shared_ptr<System::Context>&);

                void setFileExtensions(const std::set<std::string>&);

                const System::File::Path& getPath() const;

                void setPath(const System::File::Path &);

                void setCallback(const std::function<void(const std::vector<System::File::Info>&)>&);
                void setCancelCallback(const std::function<void(void)>&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init &) override;

            private:
                std::string _getItemCountLabel(size_t) const;

                void _optionsUpdate();
                void _selectedUpdate();

                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv

