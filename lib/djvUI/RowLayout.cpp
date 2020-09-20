// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/RowLayout.h>

#include <djvUI/LayoutUtil.h>
#include <djvUI/Separator.h>
#include <djvUI/Spacer.h>
#include <djvUI/TextBlock.h>

#include <djvMath/Math.h>

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

            void Row::_init(Orientation orientation, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Row");

                _p->orientation = orientation;
            }

            Row::Row() :
                _p(new Private)
            {}

            Row::~Row()
            {}

            std::shared_ptr<Row> Row::create(Orientation orientation, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Row>(new Row);
                out->_init(orientation, context);
                return out;
            }

            void Row::addSeparator()
            {
                if (auto context = getContext().lock())
                {
                    addChild(Separator::create(context));
                }
            }

            void Row::addSpacer(MetricsRole value)
            {
                if (auto context = getContext().lock())
                {
                    auto spacer = Spacer::create(_p->orientation, context);
                    spacer->setSpacerSize(value);
                    addChild(spacer);
                }
            }

            void Row::addExpander()
            {
                if (auto context = getContext().lock())
                {
                    auto widget = Widget::create(context);
                    addChild(widget);
                    setStretch(widget, RowStretch::Expand);
                }
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

            const Spacing& Row::getSpacing() const
            {
                return _p->spacing;
            }

            void Row::setSpacing(const Spacing& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacing)
                    return;
                p.spacing = value;
                _resize();
            }

            RowStretch Row::getStretch(const std::shared_ptr<Widget>& value) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.stretch.find(value);
                return i != p.stretch.end() ? i->second : RowStretch::First;
            }

            void Row::setStretch(const std::shared_ptr<Widget>& value, RowStretch stretch)
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

                float out = 0.F;

                // Get the child sizes.
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const auto& children = getChildWidgets();
                size_t visibleChildren = 0;
                for (const auto& child : children)
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
                        out += s.y * static_cast<float>(visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                out += m.y;
                return out;
            }

            void Row::addChild(const std::shared_ptr<IObject>& value)
            {
                Widget::addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    _p->stretch[widget] = RowStretch::None;
                    _resize();
                }
            }

            void Row::removeChild(const std::shared_ptr<IObject>& value)
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

            void Row::_preLayoutEvent(System::Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                size_t visibleChildren = 0;
                for (const auto& child : getChildWidgets())
                {
                    if (child->isVisible())
                    {
                        const glm::vec2& childMinimumSize = child->getMinimumSize();
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
                        minimumSize.x += s.x * static_cast<float>(visibleChildren - 1);
                        break;
                    case Orientation::Vertical:
                        minimumSize.y += s.y * static_cast<float>(visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Row::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();

                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const float gw = g.w();
                const float gh = g.h();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                size_t expandCount = 0;
                const auto& children = getChildWidgets();
                size_t visibleChildren = 0;
                for (const auto& child : children)
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
                        minimumSize.x += s.x * static_cast<float>(visibleChildren - 1);
                        break;
                    case Orientation::Vertical:
                        minimumSize.y += s.y * static_cast<float>(visibleChildren - 1);
                        break;
                    default: break;
                    }
                }

                // Calculate the geometry.
                glm::vec2 pos = g.min;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                {
                    for (const auto& child : children)
                    {
                        if (child->isVisible())
                        {
                            float cellSize = 0.F;
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
                            const Math::BBox2f cellGeometry(pos.x, pos.y, cellSize, gh);
                            Math::BBox2f childGeometry = getAlign(cellGeometry, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                            childGeometry.max.x = std::min(childGeometry.max.x, g.max.x);
                            child->setGeometry(childGeometry);
                            pos.x += cellGeometry.w() + s.x;
                        }
                    }
                    break;
                }
                case Orientation::Vertical:
                {
                    for (const auto& child : children)
                    {
                        if (child->isVisible())
                        {
                            float cellSize = 0.F;
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
                            const Math::BBox2f cellGeometry(pos.x, pos.y, gw, cellSize);
                            Math::BBox2f childGeometry = getAlign(cellGeometry, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                            childGeometry.max.y = std::min(childGeometry.max.y, g.max.y);
                            child->setGeometry(childGeometry);
                            pos.y += cellGeometry.h() + s.y;
                        }
                    }
                    break;
                }
                default: break;
                }
            }

            void Horizontal::_init(const std::shared_ptr<System::Context>& context)
            {
                Row::_init(Orientation::Horizontal, context);
            }

            Horizontal::Horizontal()
            {}

            std::shared_ptr<Horizontal> Horizontal::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Horizontal>(new Horizontal);
                out->_init(context);
                return out;
            }

            void Vertical::_init(const std::shared_ptr<System::Context>& context)
            {
                Row::_init(Orientation::Vertical, context);
            }

            Vertical::Vertical()
            {}

            std::shared_ptr<Vertical> Vertical::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Vertical>(new Vertical);
                out->_init(context);
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace ddjv

