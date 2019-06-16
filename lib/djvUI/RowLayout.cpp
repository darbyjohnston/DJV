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

#include <djvUI/RowLayout.h>

#include <djvUI/Separator.h>
#include <djvUI/Spacer.h>
#include <djvUI/TextBlock.h>

#include <djvCore/Math.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Row::Private
            {
                Orientation orientation = Orientation::Horizontal;
                Spacing spacing = Spacing(MetricsRole::Spacing, MetricsRole::Spacing);
                std::map<std::shared_ptr<Widget>, RowStretch> stretch;
            };

            void Row::_init(Orientation orientation, Context * context)
            {
                Widget::_init(context);

                _p->orientation = orientation;

                setClassName("djv::UI::Layout::Row");
            }

            Row::Row() :
                _p(new Private)
            {}

            Row::~Row()
            {}

            std::shared_ptr<Row> Row::create(Orientation orientation, Context * context)
            {
                auto out = std::shared_ptr<Row>(new Row);
                out->_init(orientation, context);
                return out;
            }

            void Row::addSeparator()
            {
                addChild(Separator::create(getContext()));
            }

            void Row::addSpacer()
            {
                auto spacer = Spacer::create(_p->orientation, getContext());
                addChild(spacer);
            }

            void Row::addExpander()
            {
                auto widget = Widget::create(getContext());
                addChild(widget);
                setStretch(widget, RowStretch::Expand);
            }

            Orientation Row::getOrientation() const
            {
                return _p->orientation;
            }

            void Row::setOrientation(Orientation value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.orientation)
                    return;
                p.orientation = value;
                _resize();
            }

            const Spacing & Row::getSpacing() const
            {
                return _p->spacing;
            }

            void Row::setSpacing(const Spacing & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacing)
                    return;
                p.spacing = value;
                _resize();
            }

            RowStretch Row::getStretch(const std::shared_ptr<Widget> & value) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.stretch.find(value);
                return i != p.stretch.end() ? i->second : RowStretch::First;
            }

            void Row::setStretch(const std::shared_ptr<Widget> & value, RowStretch stretch)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.stretch.find(value);
                if (i != p.stretch.end())
                {
                    if (stretch != i->second)
                    {
                        p.stretch[value] = stretch;
                        _resize();
                    }
                }
            }

            float Row::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();

                float out = 0.f;

                // Get the child sizes.
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const auto& children = getChildWidgets();
                size_t visibleChildren = 0;
                for (const auto & child : children)
                {
                    if (child->isVisible())
                    {
                        const float heightForWidth = child->getHeightForWidth(value - m.x);
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            out = std::max(out, heightForWidth);
                            break;
                        case Orientation::Vertical:
                            out += heightForWidth;
                            break;
                        default: break;
                        }
                        ++visibleChildren;
                    }
                }

                // Adjust for spacing.
                if (visibleChildren > 1)
                {
                    const glm::vec2 s = p.spacing.get(style);
                    switch (p.orientation)
                    {
                    case Orientation::Vertical:
                        out += s.y * (visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                out += m.y;
                return out;
            }

            void Row::addChild(const std::shared_ptr<IObject> & value)
            {
                Widget::addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    _p->stretch[widget] = RowStretch::None;
                    _resize();
                }
            }

            void Row::removeChild(const std::shared_ptr<IObject> & value)
            {
                Widget::removeChild(value);
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const auto i = p.stretch.find(widget);
                    if (i != p.stretch.end())
                    {
                        p.stretch.erase(i);
                    }
                    _resize();
                }
            }

            void Row::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                size_t visibleChildren = 0;
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible())
                    {
                        const glm::vec2 & childMinimumSize = child->getMinimumSize();
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            minimumSize.x += childMinimumSize.x;
                            minimumSize.y = std::max(minimumSize.y, childMinimumSize.y);
                            break;
                        case Orientation::Vertical:
                            minimumSize.x = std::max(minimumSize.x, childMinimumSize.x);
                            minimumSize.y += childMinimumSize.y;
                            break;
                        default: break;
                        }
                        ++visibleChildren;
                    }
                }

                // Adjust for spacing.
                const auto& style = _getStyle();
                if (visibleChildren > 1)
                {
                    const glm::vec2 s = p.spacing.get(style);
                    switch (p.orientation)
                    {
                    case Orientation::Horizontal:
                        minimumSize.x += s.x * (visibleChildren - 1);
                        break;
                    case Orientation::Vertical:
                        minimumSize.y += s.y * (visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Row::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();

                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const float gw = g.w();
                const float gh = g.h();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                size_t expandCount = 0;
                const auto& children = getChildWidgets();
                size_t visibleChildren = 0;
                for (const auto & child : children)
                {
                    if (child->isVisible())
                    {
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            switch (p.stretch[child])
                            {
                            case RowStretch::None:
                                minimumSize.x += child->getMinimumSize().x;
                                break;
                            case RowStretch::Expand:
                                ++expandCount;
                                break;
                            default: break;
                            }
                            break;
                        case Orientation::Vertical:
                            switch (p.stretch[child])
                            {
                            case RowStretch::None:
                                minimumSize.y += child->getMinimumSize().y;
                                break;
                            case RowStretch::Expand:
                                ++expandCount;
                                break;
                            default: break;
                            }
                            break;
                        default: break;
                        }
                        ++visibleChildren;
                    }
                }

                // Adjust for spacing.
                const glm::vec2 s = p.spacing.get(style);
                if (visibleChildren > 1)
                {
                    switch (p.orientation)
                    {
                    case Orientation::Horizontal:
                        minimumSize.x += s.x * (visibleChildren - 1);
                        break;
                    case Orientation::Vertical:
                        minimumSize.y += s.y * (visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                // Calculate the geometry.
                std::vector<BBox2f> childrenGeometry;
                glm::vec2 pos = g.min;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                {
                    for (const auto & child : children)
                    {
                        if (child->isVisible())
                        {
                            float cellSize = 0.f;
                            switch (p.stretch[child])
                            {
                            case RowStretch::None:
                                cellSize = child->getMinimumSize().x;
                                break;
                            case RowStretch::Expand:
                                cellSize = ceilf((gw - minimumSize.x) / static_cast<float>(expandCount));
                                break;
                            default: break;
                            }
                            const BBox2f cellGeometry(pos.x, pos.y, cellSize, gh);
                            child->setGeometry(Widget::getAlign(cellGeometry, child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                            pos.x += cellGeometry.w() + s.x;
                        }
                    }
                    break;
                }
                case Orientation::Vertical:
                {
                    for (const auto & child : children)
                    {
                        if (child->isVisible())
                        {
                            float cellSize = 0.f;
                            switch (p.stretch[child])
                            {
                            case RowStretch::None:
                                switch (child->getHAlign())
                                {
                                case HAlign::Fill:
                                    cellSize = child->getHeightForWidth(gw);
                                    break;
                                default:
                                    cellSize = child->getHeightForWidth(child->getMinimumSize().x);
                                    break;
                                }
                                break;
                            case RowStretch::Expand:
                                cellSize = ceilf((gh - minimumSize.y) / static_cast<float>(expandCount));
                                break;
                            default: break;
                            }
                            const BBox2f cellGeometry(pos.x, pos.y, gw, cellSize);
                            const BBox2f childGeometry = Widget::getAlign(cellGeometry, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                            child->setGeometry(childGeometry);
                            pos.y += cellGeometry.h() + s.y;
                        }
                    }
                    break;
                }
                default: break;
                }
            }

            void Horizontal::_init(Context * context)
            {
                Row::_init(Orientation::Horizontal, context);
            }

            Horizontal::Horizontal()
            {}

            std::shared_ptr<Horizontal> Horizontal::create(Context * context)
            {
                auto out = std::shared_ptr<Horizontal>(new Horizontal);
                out->_init(context);
                return out;
            }

            void Vertical::_init(Context * context)
            {
                Row::_init(Orientation::Vertical, context);
            }

            Vertical::Vertical()
            {}

            std::shared_ptr<Vertical> Vertical::create(Context * context)
            {
                auto out = std::shared_ptr<Vertical>(new Vertical);
                out->_init(context);
                return out;
            }

        } // namespace Layout
    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Layout,
        RowStretch,
        DJV_TEXT("None"),
        DJV_TEXT("Expand"));

} // namespace ddjv

