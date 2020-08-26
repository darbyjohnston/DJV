// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidgetPrivate.h>

#include <djvUI/Label.h>
#include <djvUI/ToolButton.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct TitleBar::Private
        {
            bool active = false;
            float maximize = 0.F;
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> maximizeButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::function<void(void)> maximizeCallback;
            std::function<void(void)> closeCallback;
        };

        void TitleBar::_init(
            const std::string& text,
            const std::string& toolTip,
            const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setText(text);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin, UI::MetricsRole::Margin, UI::MetricsRole::None, UI::MetricsRole::None));
            p.titleLabel->setTooltip(toolTip);

            p.maximizeButton = UI::ToolButton::create(context);
            p.maximizeButton->setIcon("djvIconSDI");

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.titleLabel);
            p.layout->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.layout->addChild(p.maximizeButton);
            p.layout->addChild(p.closeButton);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<TitleBar>(std::dynamic_pointer_cast<TitleBar>(shared_from_this()));
            p.maximizeButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->maximizeCallback)
                        {
                            widget->_p->maximizeCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.closeButton->setClickedCallback(
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
        }

        TitleBar::TitleBar() :
            _p(new Private)
        {}

        TitleBar::~TitleBar()
        {}

        std::shared_ptr<TitleBar> TitleBar::create(
            const std::string& text,
            const std::string& toolTip,
            const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TitleBar>(new TitleBar);
            out->_init(text, toolTip, context);
            return out;
        }

        void TitleBar::setActive(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.active)
                return;
            p.active = value;
            _widgetUpdate();
        }

        void TitleBar::setMaximize(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.maximize)
                return;
            p.maximize = value;
            _resize();
        }

        void TitleBar::setMaximizeCallback(const std::function<void(void)>& value)
        {
            _p->maximizeCallback = value;
        }

        void TitleBar::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void TitleBar::_preLayoutEvent(Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            glm::vec2 size = p.layout->getMinimumSize();
            size.y *= 1.F - p.maximize;
            _setMinimumSize(size);
        }

        void TitleBar::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void TitleBar::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.maximizeButton->setTooltip(_getText(DJV_TEXT("widget_media_maximize_tooltip")));
                p.closeButton->setTooltip(_getText(DJV_TEXT("widget_media_close_tooltip")));
            }
        }

        void TitleBar::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setTextColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.maximizeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.closeButton->setForegroundColorRole(p.active ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
        }

    } // namespace ViewApp
} // namespace djv

