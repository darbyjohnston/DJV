// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/GridLayout.h>

#include <djvUI/LayoutUtil.h>

#include <djvRender2D/Render.h>

#include <djvMath/Math.h>
#include <djvMath/Vector.h>

#include <array>
#include <unordered_map>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(GridStretch);
            
            struct Grid::Private
            {
                std::unordered_map<glm::ivec2, std::shared_ptr<Widget> > widgets;
                std::unordered_map<glm::ivec2, GridStretch> stretch;
                std::unordered_map<int, std::pair<float, float> > rowPosAndHeight;
                std::unordered_map<int, std::pair<float, float> > columnPosAndWidth;
                std::map<int, ColorRole> rowColorRoles;
                std::map<int, ColorRole> columnColorRoles;
                Spacing spacing = Spacing(MetricsRole::Spacing, MetricsRole::Spacing);
            };

            void Grid::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Grid");
            }

            Grid::Grid() :
                _p(new Private)
            {}

            Grid::~Grid()
            {}

            std::shared_ptr<Grid> Grid::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Grid>(new Grid);
                out->_init(context);
                return out;
            }

            glm::ivec2 Grid::getGridSize() const
            {
                DJV_PRIVATE_PTR();
                glm::ivec2 out(0, 0);
                for (const auto& widget : p.widgets)
                {
                    out = glm::max(widget.first + 1, out);
                }
                return out;
            }

            glm::ivec2 Grid::getGridPos(const std::shared_ptr<Widget>& value)
            {
                DJV_PRIVATE_PTR();
                glm::ivec2 out(-1, -1);
                for (auto i : p.widgets)
                {
                    if (value == i.second)
                    {
                        out = i.first;
                        break;
                    }
                }
                return out;
            }

            const Spacing& Grid::getSpacing() const
            {
                return _p->spacing;
            }

            GridStretch Grid::getStretch(const std::shared_ptr<Widget>& value) const
            {
                GridStretch out = GridStretch::First;
                DJV_PRIVATE_PTR();
                for (const auto& widget : p.widgets)
                {
                    if (value == widget.second)
                    {
                        out = p.stretch[widget.first];
                    }
                }
                return out;
            }

            void Grid::setGridPos(const std::shared_ptr<Widget>& widget, const glm::ivec2& pos, GridStretch stretch)
            {
                DJV_PRIVATE_PTR();
                for (auto i = p.widgets.begin(); i != p.widgets.end(); ++i)
                {
                    if (widget == i->second)
                    {
                        const auto j = p.stretch.find(i->first);
                        if (j != p.stretch.end())
                        {
                            p.stretch.erase(j);
                        }
                        p.widgets.erase(i);
                        break;
                    }
                }
                p.widgets[pos] = widget;
                p.stretch[pos] = stretch;
                _resize();
            }

            void Grid::setGridPos(const std::shared_ptr<Widget>& widget, int x, int y, GridStretch stretch)
            {
                setGridPos(widget, glm::ivec2(x, y), stretch);
            }

            void Grid::setSpacing(const Spacing& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacing)
                    return;
                p.spacing = value;
                _resize();
            }

            void Grid::setStretch(const std::shared_ptr<Widget>& value, GridStretch stretch)
            {
                DJV_PRIVATE_PTR();
                for (const auto& widget : p.widgets)
                {
                    if (value == widget.second)
                    {
                        if (stretch != p.stretch[widget.first])
                        {
                            p.stretch[widget.first] = stretch;
                            _resize();
                            break;
                        }
                    }
                }
            }

            void Grid::setRowColorRole(int value, ColorRole role)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.rowColorRoles.find(value);
                if (i != p.rowColorRoles.end())
                {
                    if (role != i->second)
                    {
                        p.rowColorRoles[value] = role;
                        _redraw();
                    }
                }
                else
                {
                    p.rowColorRoles[value] = role;
                    _redraw();
                }
            }

            void Grid::setColumnColorRole(int value, ColorRole role)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.columnColorRoles.find(value);
                if (i != p.columnColorRoles.end())
                {
                    if (role != i->second)
                    {
                        p.columnColorRoles[value] = role;
                        _redraw();
                    }
                }
                else
                {
                    p.columnColorRoles[value] = role;
                    _redraw();
                }
            }

            float Grid::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();

                float out = 0.F;

                // Get the child sizes.
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const glm::ivec2 gridSize = getGridSize();
                float minimumWidth = 0.F;
                std::vector<float> minimumWidths;
                std::map<Orientation, std::vector<bool> > expandList;
                std::map<Orientation, size_t> expandCount =
                {
                    { Orientation::Horizontal, 0 },
                    { Orientation::Vertical,   0 }
                };
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float minimumWidthMax = 0.F;
                    bool expand = false;
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumWidthMax = std::max(i->second->getMinimumSize().x, minimumWidthMax);
                                switch (p.stretch[i->first])
                                {
                                case GridStretch::Both:
                                case GridStretch::Horizontal: expand = true; break;
                                default: break;
                                }
                            }
                        }
                    }
                    minimumWidths.push_back(minimumWidthMax);
                    expandList[Orientation::Horizontal].push_back(expand);
                    if (expand)
                    {
                        ++expandCount[Orientation::Horizontal];
                    }
                    else
                    {
                        minimumWidth += minimumWidthMax;
                    }
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float heightForWidth = 0.F;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                float w = 0.F;
                                if (expandList[Orientation::Horizontal][x])
                                {
                                    w = ((value - m.x) - minimumWidth) / static_cast<float>(expandCount[Orientation::Horizontal]);
                                }
                                w = std::max(w, minimumWidths[x]);
                                heightForWidth = std::max(i->second->getHeightForWidth(w), heightForWidth);
                            }
                        }
                    }
                    out += heightForWidth;
                }

                // Adjust for spacing.
                const glm::vec2 s = p.spacing.get(style);
                if (gridSize.y)
                {
                    out += s.y * static_cast<float>(gridSize.y - 1);
                }

                out += m.y;
                return out;
            }

            void Grid::addChild(const std::shared_ptr<IObject>& value)
            {
                Widget::addChild(value);
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    const glm::vec2 pos(0, getGridSize().y);
                    p.widgets[pos] = widget;
                    p.stretch[pos] = GridStretch::None;
                    _resize();
                }
            }

            void Grid::removeChild(const std::shared_ptr<IObject>& value)
            {
                Widget::removeChild(value);
                DJV_PRIVATE_PTR();
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    for (auto i : p.widgets)
                    {
                        if (i.second == widget)
                        {
                            p.widgets.erase(i.first);
                            p.stretch.erase(i.first);
                            _resize();
                            break;
                        }
                    }
                }
            }
            
            void Grid::_preLayoutEvent(System::Event::PreLayout&)
            {
                DJV_PRIVATE_PTR();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                const glm::ivec2 gridSize = getGridSize();
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float minimumSizeMax = 0.F;
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
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
                    float minimumSizeMax = 0.F;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
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
                const auto& style = _getStyle();
                const glm::vec2 s = p.spacing.get(style);
                if (gridSize.x)
                {
                    minimumSize.x += s.x * static_cast<float>(gridSize.x - 1);
                }
                if (gridSize.y)
                {
                    minimumSize.y += s.y * static_cast<float>(gridSize.y - 1);
                }

                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Grid::_layoutEvent(System::Event::Layout& event)
            {
                DJV_PRIVATE_PTR();

                const auto& style = _getStyle();
                const Math::BBox2f& g = getGeometry();
                const float gw = g.w();
                const float gh = g.h();

                // Get the child sizes.
                glm::vec2 minimumSize(0.F, 0.F);
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
                    float minimumSizeMax = 0.F;
                    bool expand = false;
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getMinimumSize().x, minimumSizeMax);
                                switch (p.stretch[i->first])
                                {
                                case GridStretch::Both:
                                case GridStretch::Horizontal: expand = true; break;
                                default: break;
                                }
                            }
                        }
                    }
                    minimumSizes[Orientation::Horizontal].push_back(minimumSizeMax);
                    expandList[Orientation::Horizontal].push_back(expand);
                    if (expand)
                    {
                        ++expandCount[Orientation::Horizontal];
                    }
                    else
                    {
                        minimumSize.x += minimumSizeMax;
                    }
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float minimumSizeMax = 0.F;
                    bool expand = false;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                float w = 0.F;
                                if (expandList[Orientation::Horizontal][x])
                                {
                                    w = (gw - getMargin().getWidth(style) - minimumSize.x) / static_cast<float>(expandCount[Orientation::Horizontal]);
                                }
                                w = std::max(w, minimumSizes[Orientation::Horizontal][x]);

                                minimumSizeMax = std::max(i->second->getHeightForWidth(w), minimumSizeMax);
                                switch (p.stretch[i->first])
                                {
                                case GridStretch::Both:
                                case GridStretch::Vertical: expand = true; break;
                                default: break;
                                }
                            }
                        }
                    }
                    minimumSizes[Orientation::Vertical].push_back(minimumSizeMax);
                    expandList[Orientation::Vertical].push_back(expand);
                    if (expand)
                    {
                        ++expandCount[Orientation::Vertical];
                    }
                    else
                    {
                        minimumSize.y += minimumSizeMax;
                    }
                }

                // Adjust for spacing.
                const glm::vec2 s = p.spacing.get(style);
                if (gridSize.x)
                {
                    minimumSize.x += s.x * static_cast<float>(gridSize.x - 1);
                }
                if (gridSize.y)
                {
                    minimumSize.y += s.y * static_cast<float>(gridSize.y - 1);
                }

                // Calculate the geometry.
                p.rowPosAndHeight.clear();
                p.columnPosAndWidth.clear();
                std::map<std::shared_ptr<Widget>, Math::BBox2f> childrenGeometry;
                glm::vec2 pos(g.min.x + getMargin().get(Side::Left, style), g.min.y + getMargin().get(Side::Top, style));
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float w = 0.F;
                    if (expandList[Orientation::Horizontal][x])
                    {
                        w = (gw - getMargin().getWidth(style) - minimumSize.x) / static_cast<float>(expandCount[Orientation::Horizontal]);
                    }
                    w = std::max(w, minimumSizes[Orientation::Horizontal][x]);
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                p.columnPosAndWidth[x].first = pos.x;
                                p.columnPosAndWidth[x].second = std::max(p.columnPosAndWidth[x].second, w);
                                childrenGeometry[i->second] = Math::BBox2f(pos.x, 0.F, w, 0.F);
                            }
                        }
                    }
                    pos.x += w + s.x;
                }
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float h = 0.F;
                    if (expandList[Orientation::Vertical][y])
                    {
                        h = (gh - getMargin().getHeight(style) - minimumSize.y) / static_cast<float>(expandCount[Orientation::Vertical]);
                    }
                    h = std::max(h, minimumSizes[Orientation::Vertical][y]);
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                p.rowPosAndHeight[y].first = pos.y;
                                p.rowPosAndHeight[y].second = std::max(p.rowPosAndHeight[y].second, h);
                                const Math::BBox2f bbox = childrenGeometry[i->second];
                                childrenGeometry[i->second] = Math::BBox2f(bbox.x(), pos.y, bbox.w(), h);
                            }
                        }
                    }
                    pos.y += h + s.y;
                }

                // Set the child geometry.
                for (const auto& widget : p.widgets)
                {
                    const auto& child = widget.second;
                    child->setGeometry(
                        getAlign(childrenGeometry[widget.second], child->getMinimumSize(), child->getHAlign(), child->getVAlign()).
                        intersect(g));
                }
            }

            void Grid::_paintEvent(System::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                std::map<ColorRole, std::vector<Math::BBox2f> > rects;
                for (const auto& i : p.rowPosAndHeight)
                {
                    const auto j = p.rowColorRoles.find(i.first);
                    if (j != p.rowColorRoles.end())
                    {
                        const auto& row = i.second;
                        rects[j->second].emplace_back(Math::BBox2f(g.min.x, row.first, g.w(), row.second));
                    }
                }
                for (const auto& i : p.columnPosAndWidth)
                {
                    const auto j = p.columnColorRoles.find(i.first);
                    if (j != p.columnColorRoles.end())
                    {
                        const auto& column = i.second;
                        rects[j->second].emplace_back(Math::BBox2f(column.first, g.min.y, column.second, g.h()));
                    }
                }
                for (const auto& i : rects)
                {
                    render->setFillColor(style->getColor(i.first));
                    render->drawRects(i.second);
                }
            }

        } // namespace Layout
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Layout,
        GridStretch,
        DJV_TEXT("layout_grid_stretch_none"),
        DJV_TEXT("layout_grid_stretch_horizontal"),
        DJV_TEXT("layout_grid_stretch_vertical"),
        DJV_TEXT("layout_grid_stretch_both"));

} // namespace djv

