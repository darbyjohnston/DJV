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

#include <djvUI/RowLayout.h>

#include <djvUI/Separator.h>
#include <djvUI/Spacer.h>

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
                Spacing spacing = Spacing(Style::MetricsRole::Spacing, Style::MetricsRole::Spacing);
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

            void Row::addWidget(const std::shared_ptr<Widget>& value, RowStretch stretch)
            {
                value->setParent(shared_from_this());
                _p->stretch[value] = stretch;
            }

            void Row::insertWidget(const std::shared_ptr<Widget>& value, int index, RowStretch stretch)
            {
                value->setParent(shared_from_this(), index);
                _p->stretch[value] = stretch;
            }

            void Row::removeWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(nullptr);
                const auto i = _p->stretch.find(value);
                if (i != _p->stretch.end())
                {
                    _p->stretch.erase(i);
                }
            }

            void Row::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _p->stretch.clear();
            }

            void Row::addSeparator()
            {
                addWidget(Separator::create(getContext()));
            }

            void Row::addSpacer()
            {
                auto spacer = Spacer::create(_p->orientation, getContext());
                addWidget(spacer);
            }

            void Row::addExpander()
            {
                addWidget(Widget::create(getContext()), RowStretch::Expand);
            }

            Orientation Row::getOrientation() const
            {
                return _p->orientation;
            }

            void Row::setOrientation(Orientation value)
            {
                _p->orientation = value;
            }

            const Spacing& Row::getSpacing() const
            {
                return _p->spacing;
            }

            void Row::setSpacing(const Spacing& value)
            {
                _p->spacing = value;
            }

            RowStretch Row::getStretch(const std::shared_ptr<Widget>& value) const
            {
                const auto i = _p->stretch.find(value);
                return i != _p->stretch.end() ? i->second : RowStretch::First;
            }

            void Row::setStretch(const std::shared_ptr<Widget>& value, RowStretch stretch)
            {
                _p->stretch[value] = stretch;
            }

            float Row::getHeightForWidth(float value) const
            {
                float out = 0.f;
                if (auto style = _getStyle().lock())
                {
                    // Get the child sizes.
                    const auto children = getChildrenT<Widget>();
                    size_t visibleChildren = 0;
                    for (const auto& child : children)
                    {
                        if (child->isVisible())
                        {
                            const float heightForWidth = child->getHeightForWidth(value);
                            switch (_p->orientation)
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
                        const glm::vec2 s = _p->spacing.get(style);
                        switch (_p->orientation)
                        {
                        case Orientation::Vertical:
                            out += s.y * (visibleChildren - 1);
                            break;
                        default: break;
                        }
                    }

                    out += getMargin().getHeight(style);
                }
                return out;
            }

            void Row::_preLayoutEvent(Event::PreLayout&)
            {
                if (auto style = _getStyle().lock())
                {
                    // Get the child sizes.
                    glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                    const auto children = getChildrenT<Widget>();
                    size_t visibleChildren = 0;
                    for (const auto& child : children)
                    {
                        if (child->isVisible())
                        {
                            const glm::vec2& childMinimumSize = child->getMinimumSize();
                            switch (_p->orientation)
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
                    if (visibleChildren > 1)
                    {
                        const glm::vec2 s = _p->spacing.get(style);
                        switch (_p->orientation)
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
            }

            void Row::_layoutEvent(Event::Layout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const float gw = g.w();
                    const float gh = g.h();

                    // Get the child sizes.
                    glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                    size_t expandCount = 0;
                    const auto children = getChildrenT<Widget>();
                    size_t visibleChildren = 0;
                    for (const auto& child : children)
                    {
                        if (child->isVisible())
                        {
                            switch (_p->orientation)
                            {
                            case Orientation::Horizontal:
                                switch (_p->stretch[child])
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
                                switch (_p->stretch[child])
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
                    const glm::vec2 s = _p->spacing.get(style);
                    if (visibleChildren > 1)
                    {
                        switch (_p->orientation)
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
                    switch (_p->orientation)
                    {
                    case Orientation::Horizontal:
                    {
                        for (const auto& child : children)
                        {
                            if (child->isVisible())
                            {
                                float cellSize = 0.f;
                                switch (_p->stretch[child])
                                {
                                case RowStretch::None:
                                    cellSize = child->getMinimumSize().x;
                                    break;
                                case RowStretch::Expand:
                                    cellSize = (gw - minimumSize.x) / static_cast<float>(expandCount);
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
                        for (const auto& child : children)
                        {
                            if (child->isVisible())
                            {
                                float cellSize = 0.f;
                                switch (_p->stretch[child])
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
                                    cellSize = (gh - minimumSize.y) / static_cast<float>(expandCount);
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
            }

            void HorizontalLayout::_init(Context * context)
            {
                Row::_init(Orientation::Horizontal, context);
            }

            HorizontalLayout::HorizontalLayout()
            {}

            std::shared_ptr<HorizontalLayout> HorizontalLayout::create(Context * context)
            {
                auto out = std::shared_ptr<HorizontalLayout>(new HorizontalLayout);
                out->_init(context);
                return out;
            }

            void VerticalLayout::_init(Context * context)
            {
                Row::_init(Orientation::Vertical, context);
            }

            VerticalLayout::VerticalLayout()
            {}

            std::shared_ptr<VerticalLayout> VerticalLayout::create(Context * context)
            {
                auto out = std::shared_ptr<VerticalLayout>(new VerticalLayout);
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

