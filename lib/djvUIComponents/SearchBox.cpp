// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/SearchBox.h>

#include <djvUI/DrawUtil.h>
#include <djvUI/Icon.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct SearchBox::Private
        {
            bool border = true;
            std::shared_ptr<LineEditBase> lineEditBase;
            std::shared_ptr<Icon> searchIcon;
            std::shared_ptr<ToolButton> clearButton;
            std::shared_ptr<SoloLayout> soloLayout;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(const std::string &)> filterCallback;
        };

        void SearchBox::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::SearchBox");
            setVAlign(VAlign::Center);

            p.lineEditBase = LineEditBase::create(context);
            p.lineEditBase->setTextSizeRole(MetricsRole::SearchBox);
            
            p.searchIcon = Icon::create(context);
            p.searchIcon->setIcon("djvIconSearch");
            p.searchIcon->setIconColorRole(ColorRole::Foreground);
            
            p.clearButton = ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");
            p.clearButton->setTextFocusEnabled(false);
            p.clearButton->setBackgroundRole(ColorRole::None);
            
            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->setBackgroundRole(ColorRole::Trough);
            p.layout->addChild(p.lineEditBase);
            p.layout->setStretch(p.lineEditBase, RowStretch::Expand);
            p.soloLayout = SoloLayout::create(context);
            p.soloLayout->addChild(p.searchIcon);
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

        std::shared_ptr<SearchBox> SearchBox::create(const std::shared_ptr<Context>& context)
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

        void SearchBox::setBorder(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.border)
                return;
            p.border = value;
            _resize();
        }

        float SearchBox::getHeightForWidth(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const glm::vec2 m = getMargin().getSize(style);
            const float b = style->getMetric(MetricsRole::Border);
            float size = value - m.x;
            if (p.border)
            {
                size -= b * 2.F;
            }
            float out = p.layout->getHeightForWidth(size) + m.y;
            if (p.border)
            {
                out += b * 2.F;
            }
            return out;
        }

        void SearchBox::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            glm::vec2 size = p.layout->getMinimumSize();
            if (p.border)
            {
                size += b * 2.F;
            }
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void SearchBox::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(MetricsRole::Border);
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            BBox2f g2;
            if (p.border)
            {
                g2 = g.margin(-b * 2.F);
            }
            else
            {
                g2 = g;
            }
            _p->layout->setGeometry(g2);
        }

        void SearchBox::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            const float b = style->getMetric(UI::MetricsRole::Border);
            auto render = _getRender();
            if (p.lineEditBase->hasTextFocus())
            {
                render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                drawBorder(render, g, b * 2.F);
            }
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
            std::shared_ptr<Widget> currentWidget = p.searchIcon;
            if (!p.lineEditBase->getText().empty())
            {
                currentWidget = p.clearButton;
            }
            p.soloLayout->setCurrentWidget(currentWidget);
        }

    } // namespace UI
} // namespace djv
