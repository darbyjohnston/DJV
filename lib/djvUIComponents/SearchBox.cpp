// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/SearchBox.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/IconWidget.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct SearchBox::Private
        {
            std::shared_ptr<UI::Text::LineEditBase> lineEditBase;
            std::shared_ptr<UI::IconWidget> searchIconWidget;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::function<void(const std::string &)> filterCallback;
        };

        void SearchBox::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::SearchBox");
            setVAlign(UI::VAlign::Center);

            p.lineEditBase = UI::Text::LineEditBase::create(context);
            p.lineEditBase->setTextSizeRole(UI::MetricsRole::SearchBox);
            
            p.searchIconWidget = UI::IconWidget::create(context);
            p.searchIconWidget->setIcon("djvIconSearch");
            p.searchIconWidget->setIconColorRole(UI::ColorRole::Foreground);
            
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");
            p.clearButton->setInsideMargin(UI::MetricsRole::None);
            p.clearButton->setTextFocusEnabled(false);
            p.clearButton->setBackgroundRole(UI::ColorRole::None);
            
            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Trough);
            p.layout->addChild(p.lineEditBase);
            p.layout->setStretch(p.lineEditBase);
            p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->addChild(p.searchIconWidget);
            p.soloLayout->addChild(p.clearButton);
            p.layout->addChild(p.soloLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<SearchBox>(std::dynamic_pointer_cast<SearchBox>(shared_from_this()));
            p.lineEditBase->setFocusCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_redraw();
                }
            });

            p.lineEditBase->setTextChangedCallback(
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_doFilterCallback();
                    widget->_widgetUpdate();
                }
            });

            p.clearButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->clearFilter();
                    widget->_doFilterCallback();
                    widget->_widgetUpdate();
                }
            });
        }

        SearchBox::SearchBox() :
            _p(new Private)
        {}

        SearchBox::~SearchBox()
        {}

        std::shared_ptr<SearchBox> SearchBox::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SearchBox>(new SearchBox);
            out->_init(context);
            return out;
        }

        const std::string & SearchBox::getFilter() const
        {
            return _p->lineEditBase->getText();
        }

        void SearchBox::setFilter(const std::string & value)
        {
            _p->lineEditBase->setText(value);
        }

        void SearchBox::clearFilter()
        {
            _p->lineEditBase->setText(std::string());
        }

        void SearchBox::setFilterCallback(const std::function<void(const std::string &)> & value)
        {
            _p->filterCallback = value;
        }

        float SearchBox::getHeightForWidth(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const glm::vec2 m = getMargin().getSize(style);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            float size = value - m.x - btf * 2.F;
            float out = p.layout->getHeightForWidth(size);
            return out + b * 2.F + btf * 2.F + m.y;
        }

        void SearchBox::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            glm::vec2 size = p.layout->getMinimumSize();
            _setMinimumSize(size + b * 2.F + btf * 2.F + getMargin().getSize(style));
        }

        void SearchBox::_layoutEvent(System::Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const Math::BBox2f g2 = g.margin(-b - btf);
            _p->layout->setGeometry(g2);
        }

        void SearchBox::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            const auto& render = _getRender();
            if (p.lineEditBase->hasTextFocus())
            {
                render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                UI::drawBorder(render, g, btf);
            }
            Math::BBox2f g2 = g.margin(-btf);
            render->setFillColor(style->getColor(UI::ColorRole::Border));
            UI::drawBorder(render, g2, b);
        }

        void SearchBox::_doFilterCallback()
        {
            DJV_PRIVATE_PTR();
            if (p.filterCallback)
            {
                p.filterCallback(p.lineEditBase->getText());
            }
        }

        void SearchBox::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<Widget> currentWidget = p.searchIconWidget;
            if (!p.lineEditBase->getText().empty())
            {
                currentWidget = p.clearButton;
            }
            p.soloLayout->setCurrentWidget(currentWidget);
        }

    } // namespace UIComponents
} // namespace djv
