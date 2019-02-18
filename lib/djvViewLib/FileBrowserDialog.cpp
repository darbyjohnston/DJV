//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/FileBrowserDialog.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvUI/MDICanvas.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct FileBrowserDialog::Private
        {
			bool shown = false;
			std::shared_ptr<UI::FileBrowser::Widget> fileBrowser;
            std::function<void(const Core::FileSystem::FileInfo &)> callback;
        };

        void FileBrowserDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
			p.fileBrowser = UI::FileBrowser::Widget::create(context);
			p.fileBrowser->setPath(Core::FileSystem::Path("."));
			addWidget(p.fileBrowser, UI::RowStretch::Expand);

            auto weak = std::weak_ptr<FileBrowserDialog>(std::dynamic_pointer_cast<FileBrowserDialog>(shared_from_this()));
            p.fileBrowser->setCallback(
                [weak](const Core::FileSystem::FileInfo & value)
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

		FileBrowserDialog::FileBrowserDialog() :
            _p(new Private)
        {}

		FileBrowserDialog::~FileBrowserDialog()
        {}

        std::shared_ptr<FileBrowserDialog> FileBrowserDialog::create(Context * context)
        {
            auto out = std::shared_ptr<FileBrowserDialog>(new FileBrowserDialog);
            out->_init(context);
            return out;
        }

        void FileBrowserDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
        {
            _p->callback = value;
        }

        void FileBrowserDialog::_localeEvent(Event::Locale & event)
        {
			IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("File Browser")));
        }

    } // namespace ViewLib
} // namespace djv

