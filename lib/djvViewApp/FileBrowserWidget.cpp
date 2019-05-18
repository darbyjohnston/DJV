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

#include <djvViewApp/FileBrowserWidget.h>

#include <djvUIComponents/FileBrowser.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct FileBrowserWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::FileBrowser::Widget> fileBrowser;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(const Core::FileSystem::FileInfo &)> callback;
            std::function<void(void)> closeCallback;
        };

        void FileBrowserWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::FileBrowserWidget");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);
            auto closeButton = UI::ToolButton::create(context);
            closeButton->setIcon("djvIconClose");

            p.fileBrowser = UI::FileBrowser::Widget::create(context);
            p.fileBrowser->setPath(Core::FileSystem::Path("."));

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->setBackgroundRole(UI::ColorRole::BackgroundHeader);
            hLayout->addChild(p.titleLabel);
            hLayout->setStretch(p.titleLabel, UI::RowStretch::Expand);
            hLayout->addChild(closeButton);
            p.layout->addChild(hLayout);
            p.layout->addChild(p.fileBrowser);
            p.layout->setStretch(p.fileBrowser, UI::RowStretch::Expand);
            addChild(p.layout);

            auto weak = std::weak_ptr<FileBrowserWidget>(std::dynamic_pointer_cast<FileBrowserWidget>(shared_from_this()));
            closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->closeCallback)
                    {
                        widget->_p->closeCallback();
                    }
                }
            });

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

        FileBrowserWidget::FileBrowserWidget() :
            _p(new Private)
        {}

        FileBrowserWidget::~FileBrowserWidget()
        {}

        std::shared_ptr<FileBrowserWidget> FileBrowserWidget::create(Context * context)
        {
            auto out = std::shared_ptr<FileBrowserWidget>(new FileBrowserWidget);
            out->_init(context);
            return out;
        }

        const Core::FileSystem::Path& FileBrowserWidget::getPath() const
        {
            return _p->fileBrowser->getPath();
        }

        void FileBrowserWidget::setPath(const Core::FileSystem::Path& value)
        {
            _p->fileBrowser->setPath(value);
        }

        void FileBrowserWidget::setCallback(const std::function<void(const Core::FileSystem::FileInfo&)>& value)
        {
            _p->callback = value;
        }

        void FileBrowserWidget::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void FileBrowserWidget::_preLayoutEvent(Core::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void FileBrowserWidget::_layoutEvent(Core::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void FileBrowserWidget::_localeEvent(Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(_getText(DJV_TEXT("File Browser")));
        }

    } // namespace ViewApp
} // namespace djv

