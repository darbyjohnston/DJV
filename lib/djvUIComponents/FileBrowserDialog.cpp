//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvUI/EventSystem.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>
#include <djvCore/Path.h>

#include <list>

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

            void Dialog::_textUpdateEvent(Event::TextUpdate& event)
            {
                setTitle(_getText(DJV_TEXT("File Browser")));
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
