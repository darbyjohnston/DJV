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

#include <djvUI/Tooltip.h>

#include <djvUI/Border.h>
#include <djvUI/Overlay.h>
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
                void _init(Context *);
                TooltipLayout();

            public:
                static std::shared_ptr<TooltipLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&, const std::weak_ptr<Widget> & anchor);
                void clearWidgets();

            protected:
                void _layoutEvent(Core::Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Widget> > _widgetToAnchors;
            };

            void TooltipLayout::_init(Context * context)
            {
                Widget::_init(context);
                setClassName("djv::UI::TooltipLayout");
            }

            TooltipLayout::TooltipLayout()
            {}

            std::shared_ptr<TooltipLayout> TooltipLayout::create(Context * context)
            {
                auto out = std::shared_ptr<TooltipLayout>(new TooltipLayout);
                out->_init(context);
                return out;
            }

            void TooltipLayout::addWidget(const std::shared_ptr<Widget>& value, const std::weak_ptr<Widget> & anchor)
            {
                value->setParent(shared_from_this());
                _widgetToAnchors[value] = anchor;
                _resize();
            }

            void TooltipLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _widgetToAnchors.clear();
                _resize();
            }

            void TooltipLayout::_layoutEvent(Event::Layout&)
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f & g = getGeometry();
                    const float to = style->getMetric(Style::MetricsRole::TooltipOffset);
                    for (auto i : _widgetToAnchors)
                    {
                        if (auto anchor = i.second.lock())
                        {
                            const BBox2f & anchorBBox = anchor->getGeometry();
                            const glm::vec2 & minimumSize = i.first->getMinimumSize();
                            std::vector<BBox2f> geomCandidates;
                            const BBox2f belowRight(
                                anchorBBox.min.x,
                                anchorBBox.max.y + to,
                                minimumSize.x,
                                minimumSize.y);
                            const BBox2f aboveRight(
                                anchorBBox.min.x,
                                anchorBBox.min.y - to - minimumSize.y,
                                minimumSize.x,
                                minimumSize.y);
                            const BBox2f belowLeft(
                                anchorBBox.max.x - minimumSize.x,
                                anchorBBox.max.y + to,
                                minimumSize.x,
                                minimumSize.y);
                            const BBox2f aboveLeft(
                                anchorBBox.max.x - minimumSize.x,
                                anchorBBox.min.y - to - minimumSize.y,
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
                }
            }

            void TooltipLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(Style::MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Shadow)));
                        for (const auto & i : getChildrenT<Widget>())
                        {
                            BBox2f g = i->getGeometry();
                            g.min.x += s;
                            g.min.y += s;
                            g.max.x += s;
                            g.max.y += s;
                            render->drawRect(g);
                        }
                    }
                }
            }

        } // namespace

        struct Tooltip::Private
        {
            std::shared_ptr<Layout::Overlay> overlay;
            std::weak_ptr<Widget> anchor;
        };

        void Tooltip::_init(
            const std::weak_ptr<Widget> & anchor,
            const std::string & text,
            Context * context)
        {
            if (auto widget = anchor.lock())
            {
                if (auto window = widget->getWindow().lock())
                {
                    auto textBlock = TextBlock::create(text, context);
                    textBlock->setTextColorRole(Style::ColorRole::ForegroundTooltip);
                    textBlock->setBackgroundRole(Style::ColorRole::BackgroundTooltip);
                    textBlock->setMargin(Style::MetricsRole::Margin);

                    auto border = Layout::Border::create(context);
                    border->addWidget(textBlock);

                    auto layout = TooltipLayout::create(context);
                    layout->addWidget(border, anchor);

                    DJV_PRIVATE_PTR();
                    p.overlay = Layout::Overlay::create(context);
                    p.overlay->setCapturePointer(false);
                    p.overlay->setCaptureKeyboard(false);
                    p.overlay->setBackgroundRole(Style::ColorRole::None);
                    p.overlay->addWidget(layout);

                    window->addWidget(p.overlay);
                    p.overlay->show();
                    p.anchor = anchor;
                }
            }
        }

        Tooltip::Tooltip() :
            _p(new Private)
        {}

        Tooltip::~Tooltip()
        {
            DJV_PRIVATE_PTR();
            if (auto window = p.overlay->getWindow().lock())
            {
                window->removeWidget(p.overlay);
            }
        }

        std::shared_ptr<Tooltip> Tooltip::create(
            const std::weak_ptr<Widget> & widget,
            const std::string & text,
            Context * context)
        {
            auto out = std::shared_ptr<Tooltip>(new Tooltip);
            out->_init(widget, text, context);
            return out;
        }

    } // namespace UI
} // namespace djv
