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
#include <djvUI/ListButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MenuLayout : public Widget
            {
                DJV_NON_COPYABLE(MenuLayout);

            protected:
                void _init(Context *);
                MenuLayout();

            public:
                static std::shared_ptr<MenuLayout> create(Context *);

                bool isEmpty() const;

                void addWidget(const std::shared_ptr<Widget>&, const BBox2f &, Orientation);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();

                void layoutEvent(Event::Layout&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::pair<BBox2f, Orientation> > _widgetToAnchor;
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

            bool MenuLayout::isEmpty() const
            {
                return !getChildren().size();
            }

            void MenuLayout::addWidget(const std::shared_ptr<Widget>& widget, const BBox2f & anchor, Orientation anchorOrientation)
            {
                widget->setParent(shared_from_this());
                _widgetToAnchor[widget] = std::make_pair(anchor, anchorOrientation);
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

            void MenuLayout::layoutEvent(Event::Layout&)
            {
                for (const auto & i : _widgetToAnchor)
                {
                    const auto anchorBBox = i.second.first;
                    const auto minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
                    switch (i.second.second)
                    {
                    case Orientation::Horizontal:
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
                        geomCandidates.push_back(aboveLeft);
                        geomCandidates.push_back(aboveRight);
                        geomCandidates.push_back(belowLeft);
                        geomCandidates.push_back(belowRight);
                        break;
                    }
                    case Orientation::Vertical:
                    {
                        const BBox2f aboveLeft(
                            glm::vec2(anchorBBox.max.x - minimumSize.x, anchorBBox.min.y - minimumSize.y),
                            glm::vec2(anchorBBox.max.x, anchorBBox.min.y));
                        const BBox2f aboveRight(
                            glm::vec2(anchorBBox.min.x, anchorBBox.min.y - minimumSize.y),
                            glm::vec2(anchorBBox.min.x + minimumSize.x, anchorBBox.min.y));
                        const BBox2f belowLeft(
                            glm::vec2(anchorBBox.max.x - minimumSize.x, anchorBBox.max.y),
                            glm::vec2(anchorBBox.max.x, anchorBBox.max.y + minimumSize.y));
                        const BBox2f belowRight(
                            glm::vec2(anchorBBox.min.x, anchorBBox.max.y),
                            glm::vec2(anchorBBox.min.x + minimumSize.x, anchorBBox.max.y + minimumSize.y));
                        geomCandidates.push_back(aboveLeft);
                        geomCandidates.push_back(aboveRight);
                        geomCandidates.push_back(belowLeft);
                        geomCandidates.push_back(belowRight);
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
                        i.first->move(geomCandidates.back().min);
                        i.first->resize(geomCandidates.back().getSize());
                    }
                }
            }

        } // namespace

        struct Menu::Private
        {
            Context * context = nullptr;
            std::shared_ptr<ValueSubject<std::string> > name;
            std::vector<std::shared_ptr<Action> > actions;
            std::weak_ptr<Window> window;
            BBox2f anchorBBox;
            Orientation anchorOrientation = Orientation::First;
            std::shared_ptr<Layout::Overlay> overlay;
            std::shared_ptr<MenuLayout> menuLayout;
            std::shared_ptr<Layout::VerticalLayout> layout;
            std::vector<std::shared_ptr<ValueObserver<std::string> > > textObservers;
        };

        void Menu::_init(Context * context)
        {
            _p->context = context;
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
            return _p->name;
        }

        void Menu::setMenuName(const std::string & value)
        {
            _p->name->setIfChanged(value);
        }

        void Menu::addAction(const std::shared_ptr<Action> & action)
        {
            _p->actions.push_back(action);
        }

        void Menu::clear()
        {
            _p->actions.clear();
        }

        void Menu::show(const std::shared_ptr<Window> & window, const BBox2f & bbox, Orientation orientation)
        {
            _p->window = window;
            _p->anchorBBox = bbox;
            _p->anchorOrientation = orientation;
           
            _p->menuLayout = MenuLayout::create(_p->context);

            _p->overlay = Layout::Overlay::create(_p->context);
            _p->overlay->setCaptureKeyboard(false);
            _p->overlay->setBackgroundRole(Style::ColorRole::None);
            _p->overlay->addWidget(_p->menuLayout);
            window->addWidget(_p->overlay);
            _p->overlay->show();

            auto weak = std::weak_ptr<Menu>(shared_from_this());
            _p->overlay->setCloseCallback(
                [weak]
            {
                if (auto menu = weak.lock())
                {
                    if (auto window = menu->_p->window.lock())
                    {
                        window->removeWidget(menu->_p->overlay);
                        window = nullptr;
                    }
                    menu->_p->overlay = nullptr;
                    menu->_p->textObservers.clear();
                }
            });

            _p->layout = Layout::VerticalLayout::create(_p->context);
            _p->layout->setBackgroundRole(Style::ColorRole::Background);
            for (const auto & action : _p->actions)
            {
                auto button = Button::List::create(_p->context);
                _p->layout->addWidget(button);
                _p->textObservers.push_back(ValueObserver<std::string>::create(
                    action->getText(),
                    [button](const std::string & value)
                {
                    button->setText(value);
                }));
            }
            _p->menuLayout->addWidget(_p->layout, bbox, orientation);
        }

        void Menu::hide()
        {
            if (_p->overlay)
            {
                _p->overlay->close();
            }
        }

    } // namespace UI
} // namespace djv
