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

#include <djvUI/Tooltip.h>

#include <djvUI/Overlay.h>
#include <djvUI/Style.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvAV/Render2D.h>

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
                void _init(const std::shared_ptr<Core::Context>&);
                TooltipLayout();

            public:
                static std::shared_ptr<TooltipLayout> create(const std::shared_ptr<Core::Context>&);

                void setPos(const std::shared_ptr<Widget> &, const glm::vec2 & pos);

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _layoutEvent(Core::Event::Layout &) override;
                void _paintEvent(Event::Paint &) override;

            private:
                std::map<std::shared_ptr<Widget>, glm::vec2> _widgetToPos;
            };

            void TooltipLayout::_init(const std::shared_ptr<Core::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::TooltipLayout");
            }

            TooltipLayout::TooltipLayout()
            {}

            std::shared_ptr<TooltipLayout> TooltipLayout::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<TooltipLayout>(new TooltipLayout);
                out->_init(context);
                return out;
            }

            void TooltipLayout::setPos(const std::shared_ptr<Widget> & value, const glm::vec2 & pos)
            {
                _widgetToPos[value] = pos;
                _resize();
            }

            void TooltipLayout::addChild(const std::shared_ptr<IObject>& value)
            {
                Widget::addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    _widgetToPos[widget] = glm::vec2(0.f, 0.f);
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

            void TooltipLayout::_layoutEvent(Event::Layout &)
            {
                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                const float to = style->getMetric(MetricsRole::TooltipOffset);
                for (auto i : _widgetToPos)
                {
                    const glm::vec2 & minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
                    const BBox2f belowRight(
                        i.second.x + to,
                        i.second.y + to,
                        minimumSize.x,
                        minimumSize.y);
                    const BBox2f aboveRight(
                        i.second.x + to,
                        i.second.y - minimumSize.y - to,
                        minimumSize.x,
                        minimumSize.y);
                    const BBox2f belowLeft(
                        i.second.x - minimumSize.x - to,
                        i.second.y + to,
                        minimumSize.x,
                        minimumSize.y);
                    const BBox2f aboveLeft(
                        i.second.x - minimumSize.x - to,
                        i.second.y - minimumSize.y - to,
                        minimumSize.x,
                        minimumSize.y);
                    geomCandidates.push_back(belowRight);
                    geomCandidates.push_back(aboveRight);
                    geomCandidates.push_back(belowLeft);
                    geomCandidates.push_back(aboveLeft);
                    std::sort(geomCandidates.begin(), geomCandidates.end(),
                        [g](const BBox2f & a, const BBox2f & b) -> bool
                    {
                        return a.intersect(g).getArea() > b.intersect(g).getArea();
                    });
                    i.first->setGeometry(geomCandidates.front());
                }
            }

            void TooltipLayout::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto & i : getChildWidgets())
                {
                    BBox2f g = i->getGeometry();
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
        };

        void Tooltip::_init(
            const std::shared_ptr<Window> & window,
            const glm::vec2 & pos,
            const std::shared_ptr<Widget> & widget,
            const std::shared_ptr<Core::Context>& context)
        {
            auto layout = TooltipLayout::create(context);
            layout->addChild(widget);
            layout->setPos(widget, pos);

            DJV_PRIVATE_PTR();
            p.overlay = Layout::Overlay::create(context);
            p.overlay->setCapturePointer(false);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setBackgroundRole(ColorRole::None);
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
            const std::shared_ptr<Window> & window,
            const glm::vec2 & pos,
            const std::shared_ptr<Widget> & widget,
            const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<Tooltip>(new Tooltip);
            out->_init(window, pos, widget, context);
            return out;
        }

    } // namespace UI
} // namespace djv
