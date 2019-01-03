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
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
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

                bool isChecked() const;
                void setChecked(bool);
                void setCheckedCallback(const std::function<void(bool)>&);

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

                bool _checked = false;
                std::shared_ptr<Label> _label;
                std::shared_ptr<Icon> _icon;
                std::shared_ptr<Layout::Border> _border;
                std::map<Event::PointerID, bool> _pointerHover;
                std::function<void(bool)> _checkedCallback;
            };

            void ComboBoxButton::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ComboBoxButton");
                setBackgroundRole(Style::ColorRole::Button);
                setPointerEnabled(true);

                _label = Label::create(context);

                _icon = Icon::create(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconComboBox90DPI.png"), context);

                auto layout = Layout::Horizontal::create(context);
                layout->setMargin(Layout::Margin(Style::MetricsRole::Margin, Style::MetricsRole::Margin, Style::MetricsRole::MarginSmall, Style::MetricsRole::MarginSmall));
                layout->addWidget(_label);
                layout->addWidget(_icon);

                _border = Layout::Border::create(context);
                _border->addWidget(layout);
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

            bool ComboBoxButton::isChecked() const
            {
                return _checked;
            }

            void ComboBoxButton::setChecked(bool value)
            {
                if (value == _checked)
                    return;
                _checked = value;
                _redraw();
                if (_checkedCallback)
                {
                    _checkedCallback(_checked);
                }
            }

            void ComboBoxButton::setCheckedCallback(const std::function<void(bool)> & callback)
            {
                _checkedCallback = callback;
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
                        if (_isHovered() || _checked)
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
                setChecked(!_checked);
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

        } // namespace

        struct ComboBox::Private
        {
            std::vector<std::string> items;
            int currentItem = -1;
            std::shared_ptr<ActionGroup> actionGroup;
            std::shared_ptr<ComboBoxButton> button;
            std::shared_ptr<Menu> menu;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(int)> callback;
        };

        void ComboBox::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::UI::ComboBox");

            _p->actionGroup = ActionGroup::create(ButtonType::Radio);

            _p->button = ComboBoxButton::create(context);
            _p->button->setParent(shared_from_this());

            _p->menu = Menu::create(context);

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

            _p->button->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->menu->hide();
                    if (value && widget->_p->currentItem >= 0 && widget->_p->currentItem < widget->_p->items.size())
                    {
                        if (auto window = widget->getWindow().lock())
                        {
                            const auto & g = widget->getGeometry();
                            widget->_p->menu->popup(window, weak, MenuType::ComboBox);
                        }
                    }
                }
            });

            _p->menu->setCloseCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setChecked(false);
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
