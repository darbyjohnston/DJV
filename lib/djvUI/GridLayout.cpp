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

#include <djvUI/GridLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/Math.h>

#include <unordered_map>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Grid::Private
            {
                std::unordered_map<glm::ivec2, std::shared_ptr<Widget> > widgets;
                std::unordered_map<glm::ivec2, GridStretch> stretch;
                std::map<int, ColorRole> rowBackgroundRoles;
                std::map<int, ColorRole> columnBackgroundRoles;
                Spacing spacing = Spacing(MetricsRole::Spacing, MetricsRole::Spacing);
            };

            void Grid::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Grid");
            }

            Grid::Grid() :
                _p(new Private)
            {}

            Grid::~Grid()
            {}

            std::shared_ptr<Grid> Grid::create(Context * context)
            {
                auto out = std::shared_ptr<Grid>(new Grid);
                out->_init(context);
                return out;
            }

            glm::ivec2 Grid::getGridSize() const
            {
                glm::ivec2 out = glm::ivec2(0, 0);
                for (const auto & widget : _p->widgets)
                {
                    out = glm::max(widget.first + 1, out);
                }
                return out;
            }

            glm::ivec2 Grid::getGridPos(const std::shared_ptr<Widget> & value)
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

            void Grid::setGridPos(const std::shared_ptr<Widget> & widget, const glm::ivec2 & pos, GridStretch stretch)
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

            void Grid::setGridPos(const std::shared_ptr<Widget> & widget, int x, int y, GridStretch stretch)
            {
                setGridPos(widget, glm::ivec2(x, y), stretch);
            }

            const Spacing & Grid::getSpacing() const
            {
                return _p->spacing;
            }

            void Grid::setSpacing(const Spacing & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacing)
                    return;
                p.spacing = value;
                _resize();
            }

            GridStretch Grid::getStretch(const std::shared_ptr<Widget> & value) const
            {
                GridStretch out = GridStretch::First;
                DJV_PRIVATE_PTR();
                for (const auto & widget : p.widgets)
                {
                    if (value == widget.second)
                    {
                        out = p.stretch[widget.first];
                    }
                }
                return out;
            }

            void Grid::setStretch(const std::shared_ptr<Widget> & value, GridStretch stretch)
            {
                DJV_PRIVATE_PTR();
                for (const auto & widget : p.widgets)
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

            void Grid::setRowBackgroundRole(int value, ColorRole role)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.rowBackgroundRoles.find(value);
                if (i != p.rowBackgroundRoles.end())
                {
                    if (role != i->second)
                    {
                        p.rowBackgroundRoles[value] = role;
                        _redraw();
                    }
                }
            }

            void Grid::setColumnBackgroundRole(int value, ColorRole role)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.columnBackgroundRoles.find(value);
                if (i != p.columnBackgroundRoles.end())
                {
                    if (role != i->second)
                    {
                        p.columnBackgroundRoles[value] = role;
                        _redraw();
                    }
                }
            }

            float Grid::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();

                float out = 0.f;

                // Get the child sizes.
                auto style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                const glm::ivec2 gridSize = getGridSize();
                for (int y = 0; y < gridSize.y; ++y)
                {
                    float heightForWidth = 0.f;
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                heightForWidth = std::max(i->second->getHeightForWidth(value - m.x), heightForWidth);
                            }
                        }
                    }
                    out += heightForWidth;
                }

                // Adjust for spacing.
                const glm::vec2 s = p.spacing.get(style);
                if (gridSize.y)
                {
                    out += s.y * (gridSize.y - 1);
                }

                out += m.y;
                return out;
            }

            void Grid::addChild(const std::shared_ptr<IObject> & value)
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

            void Grid::removeChild(const std::shared_ptr<IObject> & value)
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

            void Grid::_preLayoutEvent(Event::PreLayout &)
            {
                DJV_PRIVATE_PTR();

                // Get the child sizes.
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                const glm::ivec2 gridSize = getGridSize();
                for (int x = 0; x < gridSize.x; ++x)
                {
                    float minimumSizeMax = 0.f;
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
                    float minimumSizeMax = 0.f;
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
                auto style = _getStyle();
                const glm::vec2 s = p.spacing.get(style);
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

            void Grid::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();

                const BBox2f & g = getGeometry();
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
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                minimumSizeMax = std::max(i->second->getHeightForWidth(gw), minimumSizeMax);
                                switch (p.stretch[i->first])
                                {
                                case GridStretch::Both:
                                case GridStretch::Vertical: expand = true; break;
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
                auto style = _getStyle();
                const glm::vec2 s = p.spacing.get(style);
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
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
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
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
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
                for (const auto & widget : p.widgets)
                {
                    const auto & child = widget.second;
                    child->setGeometry(Widget::getAlign(childrenGeometry[widget.second], child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                }
            }

            void Grid::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);

                auto style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);

                const auto bg = getBackgroundRole();
                auto render = _getRender();
                if (bg != ColorRole::None)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(bg)));
                    render->drawRect(g);
                }

                DJV_PRIVATE_PTR();
                std::map<int, BBox2f> rowGeom;
                const glm::ivec2 gridSize = getGridSize();
                for (int y = 0; y < gridSize.y; ++y)
                {
                    for (int x = 0; x < gridSize.x; ++x)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                const BBox2f & g1 = i->second->getGeometry();
                                rowGeom[y] = BBox2f(g.min.x, g1.min.y, g.w(), g1.h());
                                break;
                            }
                        }
                    }
                }
                for (auto i : rowGeom)
                {
                    const auto j = p.rowBackgroundRoles.find(i.first);
                    if (j != p.rowBackgroundRoles.end())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(j->second)));
                        render->drawRect(i.second);
                    }
                }

                std::map<int, BBox2f> columnGeom;
                for (int x = 0; x < gridSize.x; ++x)
                {
                    for (int y = 0; y < gridSize.y; ++y)
                    {
                        const auto i = p.widgets.find(glm::ivec2(x, y));
                        if (i != p.widgets.end())
                        {
                            if (i->second->isVisible())
                            {
                                const BBox2f & g1 = i->second->getGeometry();
                                columnGeom[y] = BBox2f(g1.min.x, g.min.y, g1.w(), g.h());
                                break;
                            }
                        }
                    }
                }
                for (auto i : columnGeom)
                {
                    const auto j = p.columnBackgroundRoles.find(i.first);
                    if (j != p.columnBackgroundRoles.end())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(j->second)));
                        render->drawRect(i.second);
                    }
                }
            }

        } // namespace Layout
    } // namespace UI    

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UI::Layout,
        GridStretch,
        DJV_TEXT("None"),
        DJV_TEXT("Horizontal"),
        DJV_TEXT("Vertical"),
        DJV_TEXT("Both"));

} // namespace djv

