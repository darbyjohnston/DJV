// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct Dialog::Private
            {
                std::shared_ptr<FileBrowser> fileBrowser;
                std::function<void(const FileSystem::FileInfo&)> callback;
            };

            void Dialog::_init(const std::shared_ptr<Context>& context)
            {
                IDialog::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::FileBrowser::Dialog");

                p.fileBrowser = FileBrowser::create(context);
                p.fileBrowser->setPath(FileSystem::Path("."));
                addChild(p.fileBrowser);
                setStretch(p.fileBrowser, UI::RowStretch::Expand);

                auto weak = std::weak_ptr<Dialog>(std::dynamic_pointer_cast<Dialog>(shared_from_this()));
                p.fileBrowser->setCallback(
                    [weak](const FileSystem::FileInfo & value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(value);
                        }
                    }
                });
            }

            Dialog::Dialog() :
                _p(new Private)
            {}

            Dialog::~Dialog()
            {}

            std::shared_ptr<Dialog> Dialog::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Dialog>(new Dialog);
                out->_init(context);
                return out;
            }

            void Dialog::setFileExtensions(const std::set<std::string>& value)
            {
                _p->fileBrowser->setFileExtensions(value);
            }

            const FileSystem::Path& Dialog::getPath() const
            {
                return _p->fileBrowser->getPath();
            }

            void Dialog::setPath(const FileSystem::Path& value)
            {
                _p->fileBrowser->setPath(value);
            }

            void Dialog::setCallback(const std::function<void(const FileSystem::FileInfo&)>& value)
            {
                _p->callback = value;
            }

            void Dialog::_initEvent(Event::Init& event)
            {
                IDialog::_initEvent(event);
                if (event.getData().text)
                {
                    setTitle(_getText(DJV_TEXT("file_browser")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
