// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Tooltip.h>

#include <djvUI/ITooltipWidget.h>
#include <djvUI/Overlay.h>
#include <djvUI/Style.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvRender2D/Render.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class TooltipLayout : public Widget
            {
                DJV_NON_COPYABLE(TooltipLayout);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                TooltipLayout();

            public:
                static std::shared_ptr<TooltipLayout> create(const std::shared_ptr<System::Context>&);

                void setPos(const std::shared_ptr<Widget>&, const glm::vec2& pos);

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, glm::vec2> _widgetToPos;
            };

            void TooltipLayout::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::TooltipLayout");
            }

            TooltipLayout::TooltipLayout()
            {}

            std::shared_ptr<TooltipLayout> TooltipLayout::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<TooltipLayout>(new TooltipLayout);
                out->_init(context);
                return out;
            }

            void TooltipLayout::setPos(const std::shared_ptr<Widget>& value, const glm::vec2& pos)
            {
                _widgetToPos[value] = pos;
                _resize();
            }

            void TooltipLayout::addChild(const std::shared_ptr<IObject>& value)
            {
                Widget::addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    _widgetToPos[widget] = glm::vec2(0.F, 0.F);
                    _resize();
                }
            }

            void TooltipLayout::removeChild(const std::shared_ptr<IObject>& value)
            {
                Widget::removeChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const auto i = _widgetToPos.find(widget);
                    if (i != _widgetToPos.end())
                    {
                        _widgetToPos.erase(i);
                    }
                }
            }
            
            void TooltipLayout::_layoutEvent(System::Event::Layout&)
            {
                const Math::BBox2f& g = getGeometry();
                const auto& style = _getStyle();
                const float to = style->getMetric(MetricsRole::TooltipOffset);
                for (auto i : _widgetToPos)
                {
                    const glm::vec2& minimumSize = i.first->getMinimumSize();
                    std::vector<Math::BBox2f> geomCandidates;
                    const Math::BBox2f belowRight(
                        i.second.x + to,
                        i.second.y + to,
                        minimumSize.x,
                        minimumSize.y);
                    const Math::BBox2f aboveRight(
                        i.second.x + to,
                        i.second.y - minimumSize.y - to,
                        minimumSize.x,
                        minimumSize.y);
                    const Math::BBox2f belowLeft(
                        i.second.x - minimumSize.x - to,
                        i.second.y + to,
                        minimumSize.x,
                        minimumSize.y);
                    const Math::BBox2f aboveLeft(
                        i.second.x - minimumSize.x - to,
                        i.second.y - minimumSize.y - to,
                        minimumSize.x,
                        minimumSize.y);
                    geomCandidates.push_back(belowRight);
                    geomCandidates.push_back(aboveRight);
                    geomCandidates.push_back(belowLeft);
                    geomCandidates.push_back(aboveLeft);
                    std::sort(geomCandidates.begin(), geomCandidates.end(),
                        [g](const Math::BBox2f& a, const Math::BBox2f& b)
                    {
                        return a.intersect(g).getArea() > b.intersect(g).getArea();
                    });
                    i.first->setGeometry(geomCandidates.front());
                }
            }

            void TooltipLayout::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                const auto& render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto& i : getChildWidgets())
                {
                    Math::BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

        } // namespace

        struct Tooltip::Private
        {
            std::shared_ptr<Layout::Overlay> overlay;
            std::shared_ptr<ITooltipWidget> widget;
        };

        void Tooltip::_init(
            const std::shared_ptr<Window>& window,
            const glm::vec2& pos,
            const std::shared_ptr<ITooltipWidget>& widget,
            const std::shared_ptr<System::Context>& context)
        {
            DJV_PRIVATE_PTR();

            p.widget = widget;

            auto layout = TooltipLayout::create(context);
            layout->addChild(widget);
            layout->setPos(widget, pos);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setCapturePointer(false);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setBackgroundColorRole(ColorRole::None);
            p.overlay->addChild(layout);

            window->addChild(p.overlay);
            p.overlay->show();
        }

        Tooltip::Tooltip() :
            _p(new Private)
        {}

        Tooltip::~Tooltip()
        {
            DJV_PRIVATE_PTR();
            if (auto window = p.overlay->getWindow())
            {
                window->removeChild(p.overlay);
            }
        }

        std::shared_ptr<Tooltip> Tooltip::create(
            const std::shared_ptr<Window>& window,
            const glm::vec2& pos,
            const std::shared_ptr<ITooltipWidget>& widget,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<Tooltip>(new Tooltip);
            out->_init(window, pos, widget, context);
            return out;
        }

        const std::shared_ptr<ITooltipWidget>& Tooltip::getWidget() const
        {
            return _p->widget;
        }

    } // namespace UI
} // namespace djv
