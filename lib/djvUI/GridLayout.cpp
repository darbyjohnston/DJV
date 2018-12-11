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

#include <djvUI/GridLayout.h>

#include <djvAV/Render2DSystem.h>

#include <djvCore/Math.h>

#include <unordered_map>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct GridLayout::Private
        {
            std::unordered_map<glm::ivec2, std::shared_ptr<Widget> > widgets;
            std::unordered_map<glm::ivec2, GridLayoutStretch> stretch;
            std::map<int, ColorRole> rowBackgroundRoles;
            std::map<int, ColorRole> columnBackgroundRoles;
            Spacing spacing = Spacing(MetricsRole::Spacing, MetricsRole::Spacing);
        };
        
        void GridLayout::_init(Context * context)
        {
            Widget::_init(context);
            
            setName("djv::UI::GridLayout");
        }

        GridLayout::GridLayout() :
            _p(new Private)
        {}

        GridLayout::~GridLayout()
        {}

        std::shared_ptr<GridLayout> GridLayout::create(Context * context)
        {
            auto out = std::shared_ptr<GridLayout>(new GridLayout);
            out->_init(context);
            return out;
        }

        void GridLayout::addWidget(const std::shared_ptr<Widget>& widget, const glm::ivec2& pos, GridLayoutStretch stretch)
        {
            widget->setParent(shared_from_this());

            _p->widgets[pos] = widget;
            _p->stretch[pos] = stretch;
        }

        void GridLayout::addWidget(const std::shared_ptr<Widget>& widget, int x, int y, GridLayoutStretch stretch)
        {
            widget->setParent(shared_from_this());

            _p->widgets[glm::vec2(x, y)] = widget;
            _p->stretch[glm::vec2(x, y)] = stretch;
        }

        void GridLayout::removeWidget(const std::shared_ptr<Widget>& widget)
        {
            for (auto i : _p->widgets)
            {
                if (i.second == widget)
                {
                    _p->widgets.erase(i.first);
                    _p->stretch.erase(i.first);
                    break;
                }
            }

            widget->setParent(nullptr);
        }

        void GridLayout::clearWidgets()
        {
            auto children = getChildren();
            for (auto& child : children)
            {
                child->setParent(nullptr);
            }

            _p->widgets.clear();
            _p->stretch.clear();
        }

        glm::ivec2 GridLayout::getGridSize() const
        {
            glm::ivec2 out = glm::ivec2(0, 0);
            for (const auto& widget : _p->widgets)
            {
                out = glm::max(widget.first + 1, out);
            }
            return out;
        }

        const Spacing& GridLayout::getSpacing() const
        {
            return _p->spacing;
        }

        void GridLayout::setSpacing(const Spacing& value)
        {
            _p->spacing = value;
        }
            
        GridLayoutStretch GridLayout::getStretch(const std::shared_ptr<Widget>& value) const
        {
            GridLayoutStretch out = GridLayoutStretch::First;
            for (const auto& widget : _p->widgets)
            {
                if (value == widget.second)
                {
                    out = _p->stretch[widget.first];
                }
            }
            return out;
        }
        
        void GridLayout::setStretch(const std::shared_ptr<Widget>& value, GridLayoutStretch stretch)
        {
            for (const auto& widget : _p->widgets)
            {
                if (value == widget.second)
                {
                    _p->stretch[widget.first] = stretch;
                }
            }
        }

        void GridLayout::setRowBackgroundRole(int value, ColorRole role)
        {
            _p->rowBackgroundRoles[value] = role;
        }

        void GridLayout::setColumnBackgroundRole(int value, ColorRole role)
        {
            _p->columnBackgroundRoles[value] = role;
        }

        float GridLayout::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                // Get the child sizes.
                const glm::ivec2 gridSize = getGridSize();
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float heightForWidth = 0.f;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                heightForWidth = std::max(i->second->getHeightForWidth(value), heightForWidth);
                            }
                        }
                    }
                    out += heightForWidth;
                }

                // Adjust for spacing.
                const glm::vec2 s = _p->spacing.get(style);
                if (gridSize.y)
                {
                    out += s.y * (gridSize.y - 1);
                }

                out += getMargin().getHeight(style);
            }
            return out;
        }

        void GridLayout::_preLayoutEvent(PreLayoutEvent&)
        {
            if (auto style = _getStyle().lock())
            {
                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                const glm::ivec2 gridSize = getGridSize();
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float minimumSizeMax = 0.f;
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getMinimumSize().x, minimumSizeMax);
                            }
                        }
                    }
                    minimumSize.x += minimumSizeMax;
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float minimumSizeMax = 0.f;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getMinimumSize().y, minimumSizeMax);
                            }
                        }
                    }
                    minimumSize.y += minimumSizeMax;
                }

                // Adjust for spacing.
                const glm::vec2 s = _p->spacing.get(style);
                if (gridSize.x)
                {
                    minimumSize.x += s.x * (gridSize.x - 1);
                }
                if (gridSize.y)
                {
                    minimumSize.y += s.y * (gridSize.y - 1);
                }

                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }
        }

        void GridLayout::_layoutEvent(LayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f& g = getGeometry();
                const float gw = g.w();
                const float gh = g.h();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                std::map<Orientation, std::vector<float> > minimumSizes;
                std::map<Orientation, std::vector<bool> > expandList;
                std::map<Orientation, size_t> expandCount =
                {
                    { Orientation::Horizontal, 0 },
                    { Orientation::Vertical,   0 }
                };
                const glm::ivec2 gridSize = getGridSize();
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float minimumSizeMax = 0.f;
                    bool expand = false;
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getMinimumSize().x, minimumSizeMax);
                                switch (_p->stretch[i->first])
                                {
                                case GridLayoutStretch::Both:
                                case GridLayoutStretch::Horizontal: expand = true; break;
                                default: break;
                                }
                            }
                        }
                    }
                    expandList[Orientation::Horizontal].push_back(expand);
                    if (expand)
                    {
                        minimumSizes[Orientation::Horizontal].push_back(0.f);
                        ++expandCount[Orientation::Horizontal];
                    }
                    else
                    {
                        minimumSize.x += minimumSizeMax;
                        minimumSizes[Orientation::Horizontal].push_back(minimumSizeMax);
                    }
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float minimumSizeMax = 0.f;
                    bool expand = false;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getHeightForWidth(gw), minimumSizeMax);
                                switch (_p->stretch[i->first])
                                {
                                case GridLayoutStretch::Both:
                                case GridLayoutStretch::Vertical: expand = true; break;
                                default: break;
                                }
                            }
                        }
                    }
                    expandList[Orientation::Vertical].push_back(expand);
                    if (expand)
                    {
                        minimumSizes[Orientation::Vertical].push_back(0.f);
                        ++expandCount[Orientation::Vertical];
                    }
                    else
                    {
                        minimumSize.y += minimumSizeMax;
                        minimumSizes[Orientation::Vertical].push_back(minimumSizeMax);
                    }
                }

                // Adjust for spacing.
                const glm::vec2 s = _p->spacing.get(style);
                if (gridSize.x)
                {
                    minimumSize.x += s.x * (gridSize.x - 1);
                }
                if (gridSize.y)
                {
                    minimumSize.y += s.y * (gridSize.y - 1);
                }

                // Calculate the geometry.
                std::map<std::shared_ptr<Widget>, BBox2f> childrenGeometry;
                glm::vec2 pos(g.min.x + getMargin().get(Side::Left, style), g.min.y + getMargin().get(Side::Top, style));
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float w = 0.f;
                    if (expandList[Orientation::Horizontal][x])
                    {
                        w = (gw - getMargin().getWidth(style) - minimumSize.x) / static_cast<float>(expandCount[Orientation::Horizontal]);
                    }
                    else
                    {
                        w = minimumSizes[Orientation::Horizontal][x];
                    }
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                childrenGeometry[i->second] = BBox2f(pos.x, 0.f, w, 0.f);
                            }
                        }
                    }
                    pos.x += w + s.x;
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float h = 0.f;
                    if (expandList[Orientation::Vertical][y])
                    {
                        h = (gh - getMargin().getHeight(style) - minimumSize.y) / static_cast<float>(expandCount[Orientation::Vertical]);
                    }
                    else
                    {
                        h = minimumSizes[Orientation::Vertical][y];
                    }
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = _p->widgets.find(glm::ivec2(x, y));
                        if (i != _p->widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                const BBox2f bbox = childrenGeometry[i->second];
                                childrenGeometry[i->second] = BBox2f(bbox.x(), pos.y, bbox.w(), h);
                            }
                        }
                    }
                    pos.y += h + s.y;
                }

                // Set the child geometry.
                for (const auto& widget : _p->widgets)
                {
                    const auto& child = widget.second;
                    child->setGeometry(Widget::getAlign(childrenGeometry[widget.second], child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                }
            }
        }

        void GridLayout::_paintEvent(PaintEvent& event)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);

                    const auto bg = getBackgroundRole();
                    if (bg != ColorRole::None)
                    {
                        render->setFillColor(style->getColor(bg));
                        render->drawRectangle(g);
                    }

                    std::map<int, BBox2f> rowGeom;
                    const glm::ivec2 gridSize = getGridSize();
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        for (int x = 0; x < gridSize.x; ++x)
                        {
                            const auto i = _p->widgets.find(glm::ivec2(x, y));
                            if (i != _p->widgets.end())
                            {
                                if (i->second->isVisible())
                                {
                                    const BBox2f& g1 = i->second->getGeometry();
                                    rowGeom[y] = BBox2f(g.min.x, g1.min.y, g.w(), g1.h());
                                    break;
                                }
                            }
                        }
                    }
                    for (auto i : rowGeom)
                    {
                        const auto j = _p->rowBackgroundRoles.find(i.first);
                        if (j != _p->rowBackgroundRoles.end())
                        {
                            render->setFillColor(style->getColor(j->second));
                            render->drawRectangle(i.second);
                        }
                    }

                    std::map<int, BBox2f> columnGeom;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        for (int y = 0; y < gridSize.y; ++y)
                        {
                            const auto i = _p->widgets.find(glm::ivec2(x, y));
                            if (i != _p->widgets.end())
                            {
                                if (i->second->isVisible())
                                {
                                    const BBox2f& g1 = i->second->getGeometry();
                                    columnGeom[y] = BBox2f(g1.min.x, g.min.y, g1.w(), g.h());
                                    break;
                                }
                            }
                        }
                    }
                    for (auto i : columnGeom)
                    {
                        const auto j = _p->columnBackgroundRoles.find(i.first);
                        if (j != _p->columnBackgroundRoles.end())
                        {
                            render->setFillColor(style->getColor(j->second));
                            render->drawRectangle(i.second);
                        }
                    }
                }
            }
        }

    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI,
        GridLayoutStretch,
        DJV_TEXT("None"),
        DJV_TEXT("Horizontal"),
        DJV_TEXT("Vertical"),
        DJV_TEXT("Both"));

} // namespace djv

