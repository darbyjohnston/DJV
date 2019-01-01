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

#include <djvUI/ComboBox.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class ComboBoxButton : public Widget
            {
                DJV_NON_COPYABLE(ComboBoxButton);

            protected:
                void _init(Context *);
                ComboBoxButton();

            public:
                static std::shared_ptr<ComboBoxButton> create(Context *);

                void setText(const std::string&);

                void setCallback(const std::function<void(void)> &);

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;
                void _paintEvent(Event::Paint&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;

            private:
                bool _isHovered() const;

                std::shared_ptr<Label> _label;
                std::shared_ptr<Layout::Border> _border;
                std::map<Event::PointerID, bool> _pointerHover;
                std::function<void(void)> _callback;
            };

            void ComboBoxButton::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ComboBoxButton");
                setBackgroundRole(Style::ColorRole::Button);
                setPointerEnabled(true);

                _label = Label::create(context);
                _label->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));

                _border = Layout::Border::create(context);
                _border->addWidget(_label);
                _border->setParent(shared_from_this());
            }

            ComboBoxButton::ComboBoxButton()
            {}

            std::shared_ptr<ComboBoxButton> ComboBoxButton::create(Context * context)
            {
                auto out = std::shared_ptr<ComboBoxButton>(new ComboBoxButton);
                out->_init(context);
                return out;
            }

            void ComboBoxButton::setText(const std::string& value)
            {
                _label->setText(value);
            }

            void ComboBoxButton::setCallback(const std::function<void(void)> & callback)
            {
                _callback = callback;
            }

            void ComboBoxButton::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_border->getMinimumSize());
            }

            void ComboBoxButton::_layoutEvent(Event::Layout &)
            {
                _border->setGeometry(getGeometry());
            }

            void ComboBoxButton::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRenderSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const BBox2f& g = getGeometry();
                        if (_isHovered())
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Hover)));
                            render->drawRectangle(g);
                        }
                    }
                }
            }

            void ComboBoxButton::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                const auto id = event.getPointerInfo().id;
                _pointerHover[id] = true;
                _redraw();
            }

            void ComboBoxButton::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto i = _pointerHover.find(id);
                if (i != _pointerHover.end())
                {
                    _pointerHover.erase(i);
                    _redraw();
                }
            }

            void ComboBoxButton::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
            }

            void ComboBoxButton::_buttonPressEvent(Event::ButtonPress& event)
            {
                event.accept();
                if (_callback)
                {
                    _callback();
                }
            }

            bool ComboBoxButton::_isHovered() const
            {
                bool out = false;
                for (const auto& i : _pointerHover)
                {
                    out |= i.second;
                }
                return out;
            }

            class ComboBoxMenuLayout : public Widget
            {
                DJV_NON_COPYABLE(ComboBoxMenuLayout);

            protected:
                ComboBoxMenuLayout();

            public:
                static std::shared_ptr<ComboBoxMenuLayout> create(Context *);

                void addWidget(const std::shared_ptr<Widget>&);
                void removeWidget(const std::shared_ptr<Widget>&);
                void clearWidgets();
                void setAnchor(const std::shared_ptr<Widget>&, const BBox2f &);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::map<std::shared_ptr<Widget>, BBox2f> _widgetToAnchor;
            };

            ComboBoxMenuLayout::ComboBoxMenuLayout()
            {
                setClassName("djv::UI::ComboBoxMenuLayout");
            }

            std::shared_ptr<ComboBoxMenuLayout> ComboBoxMenuLayout::create(Context * context)
            {
                auto out = std::shared_ptr<ComboBoxMenuLayout>(new ComboBoxMenuLayout);
                out->_init(context);
                return out;
            }

            void ComboBoxMenuLayout::addWidget(const std::shared_ptr<Widget>& widget)
            {
                widget->setParent(shared_from_this());
                _widgetToAnchor[widget] = BBox2f();
            }

            void ComboBoxMenuLayout::removeWidget(const std::shared_ptr<Widget>& value)
            {
                value->setParent(nullptr);
                const auto i = _widgetToAnchor.find(value);
                if (i != _widgetToAnchor.end())
                {
                    _widgetToAnchor.erase(i);
                }
            }

            void ComboBoxMenuLayout::clearWidgets()
            {
                auto children = getChildren();
                for (auto& child : children)
                {
                    child->setParent(nullptr);
                }
                _widgetToAnchor.clear();
            }

            void ComboBoxMenuLayout::setAnchor(const std::shared_ptr<Widget>& widget, const BBox2f & anchor)
            {
                _widgetToAnchor[widget] = anchor;
            }

            void ComboBoxMenuLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f & g = getGeometry();
                for (const auto & i : _widgetToAnchor)
                {
                    const auto anchorBBox = i.second;
                    const auto minimumSize = i.first->getMinimumSize();
                    std::vector<BBox2f> geomCandidates;
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
                    std::sort(geomCandidates.begin(), geomCandidates.end(),
                        [](const BBox2f & a, const BBox2f & b) -> bool
                    {
                        return a.getArea() > b.getArea();
                    });
                    i.first->move(geomCandidates.front().min);
                    i.first->resize(geomCandidates.front().getSize());
                }
            }

            void ComboBoxMenuLayout::_paintEvent(Event::Paint& event)
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

            class ComboBoxMenu : public Widget
            {
                DJV_NON_COPYABLE(ComboBoxMenu);

            protected:
                void _init(Context *);
                ComboBoxMenu();

            public:
                static std::shared_ptr<ComboBoxMenu> create(Context *);

                void popup(const std::shared_ptr<Window> &, const BBox2f &);

                void setVisible(bool) override;
                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

            private:
                std::shared_ptr<Layout::VerticalLayout> _layout;
                std::shared_ptr<ScrollWidget> _scrollWidget;
                std::shared_ptr<Layout::Border> _border;
                std::shared_ptr<ComboBoxMenuLayout> _menuLayout;
                std::shared_ptr<Layout::Overlay> _overlay;
                std::vector<std::shared_ptr<ValueObserver<bool> > > _checkedObservers;
            };

            void ComboBoxMenu::_init(Context * context)
            {
                Widget::_init(context);

                _layout = Layout::VerticalLayout::create(context);
                _layout->setBackgroundRole(Style::ColorRole::Background);
                _layout->setSpacing(Style::MetricsRole::None);

                _scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                _scrollWidget->addWidget(_layout);

                _border = Layout::Border::create(context);
                _border->addWidget(_scrollWidget);

                _menuLayout = ComboBoxMenuLayout::create(context);
                _menuLayout->addWidget(_border);

                _overlay = Layout::Overlay::create(context);
                _overlay->setFadeIn(false);
                _overlay->setBackgroundRole(Style::ColorRole::None);
                _overlay->addWidget(_menuLayout);
                _overlay->setParent(shared_from_this());

                auto weak = std::weak_ptr<ComboBoxMenu>(std::dynamic_pointer_cast<ComboBoxMenu>(shared_from_this()));
                _overlay->setCloseCallback(
                    [weak]
                {
                    if (auto menu = weak.lock())
                    {
                        menu->hide();
                        menu->setParent(nullptr);
                        menu->_layout->clearWidgets();
                        menu->_checkedObservers.clear();
                    }
                });
            }

            ComboBoxMenu::ComboBoxMenu()
            {
                setClassName("djv::UI::ComboBoxMenu");
            }

            std::shared_ptr<ComboBoxMenu> ComboBoxMenu::create(Context * context)
            {
                auto out = std::shared_ptr<ComboBoxMenu>(new ComboBoxMenu);
                out->_init(context);
                return out;
            }

            void ComboBoxMenu::popup(const std::shared_ptr<Window> & window, const BBox2f & bbox)
            {
                _menuLayout->setAnchor(_border, bbox);

                _layout->clearWidgets();
                _checkedObservers.clear();

                for (const auto & action : getActions())
                {
                    auto button = Button::List::create(getContext());
                    button->setButtonType(action->getButtonType()->get());
                    button->setChecked(action->isChecked()->get());
                    button->setText(action->getText()->get());
                    button->setTextHAlign(TextHAlign::Left);

                    _layout->addWidget(button);

                    _checkedObservers.push_back(
                        ValueObserver<bool>::create(
                            action->isChecked(),
                            [button](bool value)
                    {
                        button->setChecked(value);
                    }));

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

                window->addWidget(std::dynamic_pointer_cast<ComboBoxMenu>(shared_from_this()));
                show();
            }

            void ComboBoxMenu::setVisible(bool value)
            {
                Widget::setVisible(value);
                _overlay->setVisible(value);
                if (!value)
                {
                    setParent(nullptr);
                    _layout->clearWidgets();
                    _checkedObservers.clear();
                }
            }

            float ComboBoxMenu::getHeightForWidth(float value) const
            {
                return _overlay->getHeightForWidth(value);
            }

            void ComboBoxMenu::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_overlay->getMinimumSize());
            }

            void ComboBoxMenu::_layoutEvent(Event::Layout& event)
            {
                _overlay->setGeometry(getGeometry());
            }

        } // namespace

        struct ComboBox::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<ActionGroup> actionGroup;
            std::shared_ptr<ComboBoxButton> button;
            std::shared_ptr<ComboBoxMenu> menu;
            std::function<void(int)> callback;
        };

        void ComboBox::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ComboBox");

            _p->actionGroup = ActionGroup::create(ButtonType::Radio);

            _p->button = ComboBoxButton::create(context);
            _p->button->setParent(shared_from_this());

            _p->menu = ComboBoxMenu::create(context);

            _updateCurrentItem();

            auto weak = std::weak_ptr<ComboBox>(std::dynamic_pointer_cast<ComboBox>(shared_from_this()));
            _p->actionGroup->setRadioCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setCurrentItem(value);
                    widget->_p->menu->hide();
                }
            });

            _p->button->setCallback(
                [weak, context]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->currentItem >= 0 && widget->_p->currentItem < widget->_p->items.size())
                    {
                        if (auto window = widget->getWindow().lock())
                        {
                            widget->_p->menu->popup(window, widget->getGeometry());
                        }
                    }
                }
            });
        }

        ComboBox::ComboBox() :
            _p(new Private)
        {}

        ComboBox::~ComboBox()
        {}

        std::shared_ptr<ComboBox> ComboBox::create(Context * context)
        {
            auto out = std::shared_ptr<ComboBox>(new ComboBox);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ComboBox> ComboBox::create(const std::vector<std::string> & items, Context * context)
        {
            auto out = ComboBox::create(context);
            out->setItems(items);
            return out;
        }

        const std::vector<std::string> & ComboBox::getItems() const
        {
            return _p->items;
        }

        void ComboBox::setItems(const std::vector<std::string> & value)
        {
            if (value == _p->items)
                return;
            _p->items = value;
            _updateItems();
            setCurrentItem(Math::clamp(_p->currentItem, 0, static_cast<int>(_p->items.size()) - 1));
        }

        int ComboBox::getCurrentItem() const
        {
            return _p->currentItem;
        }

        void ComboBox::setCurrentItem(int value)
        {
            if (value == _p->currentItem)
                return;
            _p->currentItem = value;
            _updateCurrentItem();
        }

        void ComboBox::setCallback(const std::function<void(int)> & value)
        {
            _p->callback = value;
        }

        void ComboBox::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ComboBox::_layoutEvent(Event::Layout& event)
        {
            _p->button->setGeometry(getGeometry());
        }

        void ComboBox::_updateItems()
        {
            _p->actionGroup->clearActions();
            _p->menu->clearActions();
            for (const auto & i : _p->items)
            {
                auto action = Action::create();
                action->setText(i);
                _p->actionGroup->addAction(action);
                _p->menu->addAction(action);
            }
        }

        void ComboBox::_updateCurrentItem()
        {
            if (_p->currentItem >= 0 && _p->currentItem < _p->items.size())
            {
                _p->button->setText(_p->items[_p->currentItem]);
            }
        }

    } // namespace UI
} // namespace djv
