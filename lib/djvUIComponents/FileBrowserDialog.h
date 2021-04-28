// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/IDialog.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;
            class Path;
    
        } // namespace File
    } // namespace System

    namespace UIComponents
    {
        namespace FileBrowser
        {
            //! File browser dialog.
            class Dialog : public UI::IDialog
            {
                DJV_NON_COPYABLE(Dialog);

            protected:
                void _init(UI::SelectionType, const std::shared_ptr<System::Context>&);
                Dialog();

            public:
                ~Dialog() override;

                static std::shared_ptr<Dialog> create(UI::SelectionType, const std::shared_ptr<System::Context>&);

                void setFileExtensions(const std::set<std::string>&);

                const System::File::Path& getPath() const;

                void setPath(const System::File::Path&);

                void setCallback(const std::function<void(const std::vector<System::File::Info>&)>&);

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv

