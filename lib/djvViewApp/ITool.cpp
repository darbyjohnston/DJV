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

#include <djvViewApp/ITool.h>

#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ITool::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<UI::VerticalLayout> childLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(void)> closeCallback;
        };

        void ITool::_init(Context * context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ITool");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(UI::MetricsRole::MarginSmall);

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setBackgroundRole(UI::ColorRole::BackgroundHeader);
            p.titleBar->setSpacing(UI::MetricsRole::None);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.closeButton);

            p.childLayout = UI::VerticalLayout::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setBackgroundRole(UI::ColorRole::Background);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.titleBar);
            layout->addChild(p.childLayout);
            layout->setStretch(p.childLayout, UI::RowStretch::Expand);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setMargin(UI::Layout::Margin(UI::MetricsRole::Shadow, UI::MetricsRole::Shadow, UI::MetricsRole::Handle, UI::MetricsRole::Shadow));
            p.layout->addChild(layout);
            p.layout->setStretch(layout, UI::RowStretch::Expand);
            IWidget::addChild(p.layout);

            auto weak = std::weak_ptr<ITool>(std::dynamic_pointer_cast<ITool>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->close();
                }
            });
        }

        ITool::ITool() :
            _p(new Private)
        {}

        ITool::~ITool()
        {}

        const std::string & ITool::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void ITool::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void ITool::close()
        {
            DJV_PRIVATE_PTR();
            hide();
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void ITool::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        float ITool::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void ITool::addChild(const std::shared_ptr<IObject> & value)
        {
            DJV_PRIVATE_PTR();
            p.childLayout->addChild(value);
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                p.childLayout->setStretch(widget, UI::RowStretch::Expand);
            }
        }

        void ITool::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void ITool::clearChildren()
        {
            _p->childLayout->clearChildren();
        }

        std::map<UI::MDI::Handle, std::vector<Core::BBox2f> > ITool::_getHandles() const
        {
            DJV_PRIVATE_PTR();
            auto out = IWidget::_getHandles();
            out[UI::MDI::Handle::Move][0] = p.titleBar->getGeometry();
            return out;
        }

        void ITool::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ITool::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ITool::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.closeButton->setTooltip(_getText(DJV_TEXT("Close tooltip")));
        }

    } // namespace ViewApp
} // namespace djv

