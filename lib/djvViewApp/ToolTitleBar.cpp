// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolTitleBar.h>

#include <djvViewApp/ToolSettings.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolTitleBar::Private
        {
            std::string text;
            std::shared_ptr<UI::Text::Label> label;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> layout;
        };

        void ToolTitleBar::_init(
            const std::string& text,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ToolTitleBar");
            setBackgroundColorRole(UI::ColorRole::BackgroundHeader);

            p.text = text;

            p.label = UI::Text::Label::create(context);
            p.label->setTextHAlign(UI::TextHAlign::Left);
            p.label->setMargin(UI::MetricsRole::MarginSmall);

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconCloseSmall");

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.label);
            p.layout->setStretch(p.label);
            p.layout->addChild(p.closeButton);
            addChild(p.layout);

            auto weakContext = std::weak_ptr<System::Context>(context);
            p.closeButton->setClickedCallback(
                [weakContext]
                {
                    if (auto context = weakContext.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto toolSettings = settingsSystem->getSettingsT<ToolSettings>();
                        toolSettings->setCurrentTool(-1);
                    }
                });
        }

        ToolTitleBar::ToolTitleBar() :
            _p(new Private)
        {}

        ToolTitleBar::~ToolTitleBar()
        {}

        std::shared_ptr<ToolTitleBar> ToolTitleBar::create(
            const std::string& text,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolTitleBar>(new ToolTitleBar);
            out->_init(text, context);
            return out;
        }
        
        void ToolTitleBar::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ToolTitleBar::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ToolTitleBar::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.label->setText(_getText(p.text));
                p.closeButton->setTooltip(_getText(DJV_TEXT("tool_close")));
            }
        }

    } // namespace ViewApp
} // namespace djv

