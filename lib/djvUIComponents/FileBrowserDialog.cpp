// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvSystem/Context.h>
#include <djvSystem/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            struct Dialog::Private
            {
                std::shared_ptr<FileBrowser> fileBrowser;
            };

            void Dialog::_init(UI::SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                IDialog::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::FileBrowser::Dialog");

                p.fileBrowser = FileBrowser::create(selectionType, context);
                p.fileBrowser->setPath(System::File::Path("."));
                addChild(p.fileBrowser);
                setStretch(p.fileBrowser, UI::RowStretch::Expand);

                auto weak = std::weak_ptr<Dialog>(std::dynamic_pointer_cast<Dialog>(shared_from_this()));
                p.fileBrowser->setCallback(
                    [weak](const std::vector<System::File::Info>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_doCloseCallback();
                    }
                });
            }

            Dialog::Dialog() :
                _p(new Private)
            {}

            Dialog::~Dialog()
            {}

            std::shared_ptr<Dialog> Dialog::create(UI::SelectionType selectionType, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Dialog>(new Dialog);
                out->_init(selectionType, context);
                return out;
            }

            void Dialog::setFileExtensions(const std::set<std::string>& value)
            {
                _p->fileBrowser->setFileExtensions(value);
            }

            const System::File::Path& Dialog::getPath() const
            {
                return _p->fileBrowser->getPath();
            }

            void Dialog::setPath(const System::File::Path& value)
            {
                _p->fileBrowser->setPath(value);
            }

            void Dialog::setCallback(const std::function<void(const std::vector<System::File::Info>&)>& value)
            {
                _p->fileBrowser->setCallback(value);
            }

            void Dialog::_initEvent(System::Event::Init& event)
            {
                IDialog::_initEvent(event);
                if (event.getData().text)
                {
                    setTitle(_getText(DJV_TEXT("file_browser")));
                }
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
