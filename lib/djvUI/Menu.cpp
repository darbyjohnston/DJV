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

#include <djvUI/Menu.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/ListButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            enum class MenuType
            {
                TopLevel,
                SubMenu
            };

            class MenuLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuLayout);

            protected:
                void _init(Context *);
                MenuLayout();

            public:
                static std::shared_ptr<MenuLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&, MenuType);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();
                void setAnchor(const std::shared_ptr<Widget>&, const BBox2f &);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::pair<BBox2f, MenuType> > _widgetToAnchor;
            };

            void MenuLayout::_init(Context * context)
            {
                Widget::_init(context);
            }

            MenuLayout::MenuLayout()
            {}

            std::shared_ptr<MenuLayout> MenuLayout::create(Context * context)
            {
                auto out = std::shared_ptr<MenuLayout>(new MenuLayout);
                out->_init(context);
                return out;
            }

            void MenuLayout::addWidget(const std::shared_ptr<Widget>& widget, MenuType menuType)
            {
                widget->setParent(shared_from_this());
                _widgetToAnchor[widget] = std::make_pair(BBox2f(), menuType);
            }

            void MenuLayout::removeWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(nullptr);
                const auto i = _widgetToAnchor.find(value);
                if (i != _widgetToAnchor.end())
                {
                    _widgetToAnchor.erase(i);
                }
            }

            void MenuLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _widgetToAnchor.clear();
            }

            void MenuLayout::setAnchor(const std::shared_ptr<Widget>& widget, const BBox2f & anchor)
            {
                _widgetToAnchor[widget].first = anchor;
            }

            void MenuLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToAnchor)
                {
                    const auto anchorBBox = i.second.first;
                    const auto minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
                    switch (i.second.second)
                    {
                    case MenuType::TopLevel:
                    {
                        const BBox2f aboveLeft(
                            glm::vec2(std::min(anchorBBox.max.x - minimumSize.x, anchorBBox.min.x), anchorBBox.min.y + 1 - minimumSize.y),
                            glm::vec2(anchorBBox.max.x, anchorBBox.min.y + 1));
                        const BBox2f aboveRight(
                            glm::vec2(anchorBBox.min.x, anchorBBox.min.y + 1 - minimumSize.y),
                            glm::vec2(std::max(anchorBBox.min.x + minimumSize.x, anchorBBox.max.x), anchorBBox.min.y + 1));
                        const BBox2f belowLeft(
                            glm::vec2(std::min(anchorBBox.max.x - minimumSize.x, anchorBBox.min.x), anchorBBox.max.y - 1),
                            glm::vec2(anchorBBox.max.x, anchorBBox.max.y - 1 + minimumSize.y));
                        const BBox2f belowRight(
                            glm::vec2(anchorBBox.min.x, anchorBBox.max.y - 1),
                            glm::vec2(std::max(anchorBBox.min.x + minimumSize.x, anchorBBox.max.x), anchorBBox.max.y - 1 + minimumSize.y));
                        geomCandidates.push_back(belowRight.intersect(g));
                        geomCandidates.push_back(belowLeft.intersect(g));
                        geomCandidates.push_back(aboveRight.intersect(g));
                        geomCandidates.push_back(aboveLeft.intersect(g));
                        break;
                    }
                    case MenuType::SubMenu:
                    {
                        const BBox2f aboveLeft(
                            glm::vec2(anchorBBox.min.x - minimumSize.x, anchorBBox.max.y - minimumSize.y),
                            glm::vec2(anchorBBox.min.x, anchorBBox.max.y));
                        const BBox2f belowLeft(
                            glm::vec2(anchorBBox.min.x - minimumSize.x, anchorBBox.min.y),
                            glm::vec2(anchorBBox.min.x, anchorBBox.min.y + minimumSize.y));
                        const BBox2f aboveRight(
                            glm::vec2(anchorBBox.max.x, anchorBBox.max.y - minimumSize.y),
                            glm::vec2(anchorBBox.max.x + minimumSize.x, anchorBBox.max.y));
                        const BBox2f belowRight(
                            glm::vec2(anchorBBox.max.x, anchorBBox.min.y),
                            glm::vec2(anchorBBox.max.x + minimumSize.x, anchorBBox.min.y + minimumSize.y));
                        geomCandidates.push_back(belowRight.intersect(g));
                        geomCandidates.push_back(belowLeft.intersect(g));
                        geomCandidates.push_back(aboveRight.intersect(g));
                        geomCandidates.push_back(aboveLeft.intersect(g));
                        break;
                    }
                    default: break;
                    }
                    if (geomCandidates.size())
                    {
                        std::sort(geomCandidates.begin(), geomCandidates.end(),
                            [](const BBox2f & a, const BBox2f & b) -> bool
                        {
                            return a.getArea() > b.getArea();
                        });
                        i.first->move(geomCandidates.front().min);
                        i.first->resize(geomCandidates.front().getSize());
                    }
                }
            }

            void MenuLayout::_paintEvent(Core::Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRenderSystem().lock())
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
                            render->drawRectangle(g);
                        }
                    }
                }
            }

        } // namespace

        struct Menu::Private
        {
            std::shared_ptr<ValueSubject<std::string> > menuName;
            std::shared_ptr<Layout::VerticalLayout> layout;
            std::shared_ptr<Layout::Border> border;
            std::shared_ptr<MenuLayout> menuLayout;
            std::shared_ptr<Layout::Overlay> overlay;
        };

        void Menu::_init(Context * context)
        {
            Widget::_init(context);            

            _p->menuName = ValueSubject<std::string>::create();

            _p->layout = Layout::VerticalLayout::create(context);
            _p->layout->setBackgroundRole(Style::ColorRole::Background);
            _p->layout->setSpacing(Style::MetricsRole::None);

            _p->border = Layout::Border::create(context);
            _p->border->addWidget(_p->layout);

            _p->menuLayout = MenuLayout::create(context);
            _p->menuLayout->addWidget(_p->border, MenuType::TopLevel);

            _p->overlay = Layout::Overlay::create(context);
            _p->overlay->setBackgroundRole(Style::ColorRole::None);
            _p->overlay->addWidget(_p->menuLayout);
            _p->overlay->setParent(shared_from_this());

            auto weak = std::weak_ptr<Menu>(std::dynamic_pointer_cast<Menu>(shared_from_this()));
            _p->overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    menu->hide();
                    menu->_p->layout->clearWidgets();
                }
            });
        }

        Menu::Menu() :
            _p(new Private)
        {}

        Menu::~Menu()
        {}

        std::shared_ptr<Menu> Menu::create(Context * context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Menu> Menu::create(const std::string & name, Context * context)
        {
            auto out = std::shared_ptr<Menu>(new Menu);
            out->_init(context);
            out->setMenuName(name);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Menu::getMenuName() const
        {
            return _p->menuName;
        }

        void Menu::setMenuName(const std::string & value)
        {
            _p->menuName->setIfChanged(value);
        }

        void Menu::popup(const std::shared_ptr<Window> & window, const BBox2f & bbox)
        {
            _p->menuLayout->setAnchor(_p->border, bbox);

            _p->layout->clearWidgets();

            for (const auto & action : getActions())
            {
                auto button = Button::List::create(getContext());
                button->setButtonType(action->getButtonType()->get());
                button->setChecked(action->isChecked()->get());
                button->setText(action->getText()->get());
                button->setTextHAlign(TextHAlign::Left);
                
                _p->layout->addWidget(button);

                button->setClickedCallback(
                    [action]
                {
                    action->doClickedCallback();
                });
                button->setCheckedCallback(
                    [action](bool value)
                {
                    action->setChecked(value);
                    action->doCheckedCallback();
                });
            }

            window->addWidget(std::dynamic_pointer_cast<Menu>(shared_from_this()));
            show();
        }

        void Menu::setVisible(bool value)
        {
            Widget::setVisible(value);
            _p->overlay->setVisible(value);
            if (!value)
            {
                _p->layout->clearWidgets();
            }
        }

        float Menu::getHeightForWidth(float value) const
        {
            return _p->overlay->getHeightForWidth(value);
        }

        void Menu::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->overlay->getMinimumSize());
        }

        void Menu::_layoutEvent(Event::Layout& event)
        {
            _p->overlay->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
