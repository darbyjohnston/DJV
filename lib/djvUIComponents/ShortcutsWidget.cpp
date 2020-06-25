// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/Border.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/Overlay.h>
#include <djvUI/PopupLayout.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class KeyPressWidget : public Widget
            {
                DJV_NON_COPYABLE(KeyPressWidget);

            protected:
                void _init(
                    const ShortcutData&,
                    const std::shared_ptr<Context>&);
                KeyPressWidget();

            public:
                virtual ~KeyPressWidget();

                static std::shared_ptr<KeyPressWidget> create(
                    const ShortcutData&,
                    const std::shared_ptr<Context>&);

                void setCallback(const std::function<void(const ShortcutData&)>&);

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;
                void _keyPressEvent(Event::KeyPress&) override;
                void _textFocusEvent(Event::TextFocus&) override;
                void _textFocusLostEvent(Event::TextFocusLost&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                ShortcutData _shortcut;
                std::shared_ptr<Label> _label;
                std::shared_ptr<Border> _border;
                std::function<void(const ShortcutData&)> _callback;
                Event::PointerID _pressedID = Event::invalidID;
            };

            void KeyPressWidget::_init(
                const ShortcutData& shortcut,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setPointerEnabled(true);

                _shortcut = shortcut;

                _label = Label::create(context);
                _label->setMargin(MetricsRole::MarginSmall);

                _border = Border::create(context);
                _border->setBorderSize(MetricsRole::BorderTextFocus);
                _border->setBorderColorRole(ColorRole::None);
                _border->addChild(_label);
                addChild(_border);

                _widgetUpdate();
                takeTextFocus();
            }

            KeyPressWidget::KeyPressWidget()
            {}

            KeyPressWidget::~KeyPressWidget()
            {}

            std::shared_ptr<KeyPressWidget> KeyPressWidget::create(
                const ShortcutData& shortcut,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<KeyPressWidget>(new KeyPressWidget);
                out->_init(shortcut, context);
                return out;
            }

            void KeyPressWidget::setCallback(const std::function<void(const ShortcutData&)>& value)
            {
                _callback = value;
            }

            bool KeyPressWidget::acceptFocus(TextFocusDirection)
            {
                bool out = false;
                if (isEnabled(true) && isVisible(true) && !isClipped())
                {
                    takeTextFocus();
                    out = true;
                }
                return out;
            }

            void KeyPressWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_border->getMinimumSize());
            }

            void KeyPressWidget::_layoutEvent(Event::Layout&)
            {
                _border->setGeometry(getGeometry());
            }

            void KeyPressWidget::_pointerEnterEvent(Event::PointerEnter& event)
            {
                if (!event.isRejected())
                {
                    event.accept();
                }
            }

            void KeyPressWidget::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
            }

            void KeyPressWidget::_pointerMoveEvent(Event::PointerMove& event)
            {
                event.accept();
            }

            void KeyPressWidget::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (_pressedID || !isEnabled(true))
                    return;
                event.accept();
                takeTextFocus();
            }

            void KeyPressWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == _pressedID)
                {
                    event.accept();
                    _pressedID = Event::invalidID;
                }
            }

            void KeyPressWidget::_keyPressEvent(Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                if (auto context = getContext().lock())
                {
                    if (!event.isAccepted() && hasTextFocus())
                    {
                        switch (event.getKey())
                        {
                        case 0:
                        case GLFW_KEY_ESCAPE:
                        case GLFW_KEY_ENTER:
                        case GLFW_KEY_TAB:
                        case GLFW_KEY_CAPS_LOCK:
                        case GLFW_KEY_SCROLL_LOCK:
                        case GLFW_KEY_NUM_LOCK:
                        case GLFW_KEY_LEFT_SHIFT:
                        case GLFW_KEY_LEFT_CONTROL:
                        case GLFW_KEY_LEFT_ALT:
                        case GLFW_KEY_LEFT_SUPER:
                        case GLFW_KEY_RIGHT_SHIFT:
                        case GLFW_KEY_RIGHT_CONTROL:
                        case GLFW_KEY_RIGHT_ALT:
                        case GLFW_KEY_RIGHT_SUPER:
                        case GLFW_KEY_MENU:
                            break;
                        default:
                            _shortcut.key = event.getKey();
                            _shortcut.modifiers = event.getKeyModifiers();
                            _widgetUpdate();
                            if (_callback)
                            {
                                _callback(_shortcut);
                            }
                            break;
                        }
                    }
                }
            }

            void KeyPressWidget::_textFocusEvent(Event::TextFocus& event)
            {
                event.accept();
                _border->setBorderColorRole(ColorRole::TextFocus);
            }

            void KeyPressWidget::_textFocusLostEvent(Event::TextFocusLost& event)
            {
                event.accept();
                _border->setBorderColorRole(ColorRole::None);
            }

            void KeyPressWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void KeyPressWidget::_widgetUpdate()
            {
                const auto& textSystem = _getTextSystem();
                _label->setText(ShortcutData::getText(_shortcut, textSystem));
            }

            class ShortcutWidget : public Widget
            {
                DJV_NON_COPYABLE(ShortcutWidget);

            protected:
                void _init(
                    const std::vector<ShortcutData>&,
                    const std::shared_ptr<Context>&);
                ShortcutWidget();

            public:
                virtual ~ShortcutWidget();

                static std::shared_ptr<ShortcutWidget> create(
                    const std::vector<ShortcutData>&,
                    const std::shared_ptr<Context>&);

                void setCallback(const std::function<void(const std::vector<ShortcutData>&)>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                std::vector<ShortcutData> _shortcuts;
                std::shared_ptr<VerticalLayout> _shortcutsLayout;
                std::shared_ptr<VerticalLayout> _layout;
                std::function<void(const std::vector<ShortcutData>&)> _callback;
            };

            void ShortcutWidget::_init(
                const std::vector<ShortcutData>& shortcuts,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ShortcutWidget");

                _shortcuts = shortcuts;

                auto addButton = ToolButton::create(context);
                addButton->setIcon("djvIconAdd");

                _layout = VerticalLayout::create(context);
                _layout->setSpacing(MetricsRole::None);
                auto hLayout = HorizontalLayout::create(context);
                hLayout->addExpander();
                hLayout->addChild(addButton);
                _layout->addChild(hLayout);
                _layout->addSeparator();
                _shortcutsLayout = VerticalLayout::create(context);
                _shortcutsLayout->setSpacing(MetricsRole::None);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(_shortcutsLayout);
                _layout->addChild(scrollWidget);
                _layout->setStretch(_shortcutsLayout, RowStretch::Expand);
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<ShortcutWidget>(std::dynamic_pointer_cast<ShortcutWidget>(shared_from_this()));
                addButton->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_shortcuts.push_back(ShortcutData());
                            widget->_widgetUpdate();
                            if (widget->_callback)
                            {
                                widget->_callback(widget->_shortcuts);
                            }
                        }
                    });
            }

            ShortcutWidget::ShortcutWidget()
            {}

            ShortcutWidget::~ShortcutWidget()
            {}

            std::shared_ptr<ShortcutWidget> ShortcutWidget::create(
                const std::vector<ShortcutData>& shortcuts,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ShortcutWidget>(new ShortcutWidget);
                out->_init(shortcuts, context);
                return out;
            }

            void ShortcutWidget::setCallback(const std::function<void(const std::vector<ShortcutData>&)>& value)
            {
                _callback = value;
            }

            void ShortcutWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ShortcutWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void ShortcutWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void ShortcutWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    _shortcutsLayout->clearChildren();
                    size_t index = 0;
                    for (const auto& i : _shortcuts)
                    {
                        auto keyPressWidget = KeyPressWidget::create(i, context);
                        keyPressWidget->setBackgroundRole(0 == index % 2 ? UI::ColorRole::Trough : UI::ColorRole::None);
                        _shortcutsLayout->addChild(keyPressWidget);
                        auto weak = std::weak_ptr<ShortcutWidget>(std::dynamic_pointer_cast<ShortcutWidget>(shared_from_this()));
                        keyPressWidget->setCallback(
                            [weak, index](const ShortcutData& value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    if (widget->_callback)
                                    {
                                        widget->_shortcuts[index] = value;
                                        widget->_widgetUpdate();
                                        if (widget->_callback)
                                        {
                                            widget->_callback(widget->_shortcuts);
                                        }
                                    }
                                }
                            });
                        ++index;
                    }
                }
            }

        } // namespace

        struct ShortcutsWidget::Private
        {
            std::map<std::string, std::vector<ShortcutData> > shortcuts;
            std::map<std::string, bool> shortcutsCollisions;
            std::string filter;

            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> itemsLayout;
            std::shared_ptr<SearchBox> searchBox;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<Window> window;

            std::function<void(const ShortcutDataMap&)> shortcutsCallback;
        };

        void ShortcutsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::ShortcutsWidget");

            p.buttonGroup = ButtonGroup::create(ButtonType::Push);
            p.itemsLayout = VerticalLayout::create(context);
            p.itemsLayout->setSpacing(MetricsRole::None);
            auto scrollWidget = ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->addChild(p.itemsLayout);

            p.searchBox = SearchBox::create(context);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::SpacingSmall);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, RowStretch::Expand);
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->filter = value;
                        widget->_shortcutsUpdate();
                        widget->_itemsUpdate();
                    }
                });
        }

        ShortcutsWidget::ShortcutsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ShortcutsWidget> ShortcutsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ShortcutsWidget>(new ShortcutsWidget);
            out->_init(context);
            return out;
        }

        void ShortcutsWidget::setShortcuts(const ShortcutDataMap& value)
        {
            if (value == _p->shortcuts)
                return;
            _p->shortcuts = value;
            _shortcutsUpdate();
            _itemsUpdate();
        }

        void ShortcutsWidget::setShortcutsCallback(const std::function<void(const ShortcutDataMap&)>& value)
        {
            _p->shortcutsCallback = value;
        }

        void ShortcutsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ShortcutsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ShortcutsWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                _itemsUpdate();
            }
        }

        void ShortcutsWidget::_popup(
            const std::shared_ptr<Widget>& button,
            const std::string& shortcut,
            const std::vector<ShortcutData>& shortcutData)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto shortcutWidget = ShortcutWidget::create(shortcutData, context);

                auto popupWidget = PopupWidget::create(context);
                popupWidget->addChild(shortcutWidget);
                auto popupLayout = Layout::Popup::create(context);
                popupLayout->setButton(button);
                popupLayout->addChild(popupWidget);
                auto overlay = Layout::Overlay::create(context);
                overlay->setFadeIn(false);
                overlay->setBackgroundRole(UI::ColorRole::None);
                overlay->addChild(popupLayout);
                p.window = Window::create(context);
                p.window->setBackgroundRole(UI::ColorRole::None);
                p.window->addChild(overlay);
                p.window->show();

                auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
                shortcutWidget->setCallback(
                    [weak, shortcut](const std::vector<ShortcutData>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->shortcutsCallback)
                            {
                                auto shortcuts = widget->_p->shortcuts;
                                shortcuts[shortcut] = value;
                                widget->_p->shortcutsCallback(shortcuts);
                            }
                        }
                    });
                
                overlay->setCloseCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->window->close();
                            widget->_p->window.reset();
                        }
                    });
            }
        }

        void ShortcutsWidget::_shortcutsUpdate()
        {
            DJV_PRIVATE_PTR();
            std::map<UI::ShortcutData, std::set<std::string> > taken;
            for (const auto& i : p.shortcuts)
            {
                for (const auto& j : i.second)
                {
                    taken[j].insert(i.first);
                }
            }
            p.shortcutsCollisions.clear();
            for (const auto& i : taken)
            {
                for (const auto& j : i.second)
                {
                    p.shortcutsCollisions[j] = i.second.size() > 1;
                }
            }
        }

        void ShortcutsWidget::_itemsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                p.itemsLayout->clearChildren();
                const auto& textSystem = _getTextSystem();
                size_t index = 0;
                for (const auto& i : p.shortcuts)
                {
                    const std::string text = _getText(i.first);
                    std::vector<std::string> list;
                    for (const auto& j : i.second)
                    {
                        list.push_back(ShortcutData::getText(j.key, j.modifiers, textSystem));
                    }
                    const std::string shortcutText = String::join(list, ", ");
                    if (p.filter.empty() ||
                        String::match(text, p.filter) ||
                        String::match(shortcutText, p.filter))
                    {
                        auto button = ListButton::create(context);
                        button->setText(_getText(i.first));
                        button->setBackgroundRole(0 == index % 2 ? ColorRole::Trough : ColorRole::None);
                        std::vector<std::string> list;
                        for (const auto& j : i.second)
                        {
                            list.push_back(ShortcutData::getText(j.key, j.modifiers, textSystem));
                        }
                        button->setRightsideText(shortcutText);
                        const auto j = p.shortcutsCollisions.find(i.first);
                        if (j != p.shortcutsCollisions.end() && j->second)
                        {
                            button->setBackgroundRole(ColorRole::Warning);
                        }
                        p.buttonGroup->addButton(button);
                        p.itemsLayout->addChild(button);

                        auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
                        const std::string shortcut = i.first;
                        const std::vector<ShortcutData> shortcutData = i.second;
                        button->setClickedCallback(
                            [weak, button, shortcut, shortcutData]
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_popup(button, shortcut, shortcutData);
                                }
                            });

                        ++index;
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv

