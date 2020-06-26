// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ShortcutsWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/FormLayout.h>
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

#include <djvAV/Render2D.h>

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
                    bool takeTextFocus,
                    const std::shared_ptr<Context>&);
                KeyPressWidget();

            public:
                virtual ~KeyPressWidget();

                static std::shared_ptr<KeyPressWidget> create(
                    bool takeTextFocus,
                    const std::shared_ptr<Context>&);

                void setShortcut(const ShortcutData&);
                void setCallback(const std::function<void(const ShortcutData&)>&);

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;
                void _pointerEnterEvent(Event::PointerEnter&) override;
                void _pointerLeaveEvent(Event::PointerLeave&) override;
                void _pointerMoveEvent(Event::PointerMove&) override;
                void _buttonPressEvent(Event::ButtonPress&) override;
                void _buttonReleaseEvent(Event::ButtonRelease&) override;
                void _keyPressEvent(Event::KeyPress&) override;
                void _textFocusEvent(Event::TextFocus&) override;
                void _textFocusLostEvent(Event::TextFocusLost&) override;

                void _initEvent(Event::Init&) override;
                void _updateEvent(Event::Update&) override;

            private:
                void _widgetUpdate();

                ShortcutData _shortcut;
                bool _textFocusInit = true;
                std::shared_ptr<Label> _label;
                std::function<void(const ShortcutData&)> _callback;
                Event::PointerID _pressedID = Event::invalidID;
            };

            void KeyPressWidget::_init(
                bool takeTextFocus,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djvUIComponents::KeyPressWidget");
                setPointerEnabled(true);

                _textFocusInit = takeTextFocus;

                _label = Label::create(context);
                _label->setTextHAlign(TextHAlign::Left);
                _label->setMargin(MetricsRole::MarginSmall);
                _label->setBackgroundRole(ColorRole::Trough);
                addChild(_label);

                _widgetUpdate();
            }

            KeyPressWidget::KeyPressWidget()
            {}

            KeyPressWidget::~KeyPressWidget()
            {}

            std::shared_ptr<KeyPressWidget> KeyPressWidget::create(
                bool takeTextFocus,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<KeyPressWidget>(new KeyPressWidget);
                out->_init(takeTextFocus, context);
                return out;
            }

            void KeyPressWidget::setShortcut(const ShortcutData& value)
            {
                if (value == _shortcut)
                    return;
                _shortcut = value;
                _widgetUpdate();
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
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                glm::vec2 size = _label->getMinimumSize();
                size += b * 2.F + btf * 2.F;
                _setMinimumSize(size);
            }

            void KeyPressWidget::_layoutEvent(Event::Layout&)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const BBox2f& g = getGeometry();
                const BBox2f g2 = g.margin(-(b + btf));
                _label->setGeometry(g2);
            }

            void KeyPressWidget::_paintEvent(Event::Paint&)
            {
                const auto& style = _getStyle();
                const float b = style->getMetric(MetricsRole::Border);
                const float btf = style->getMetric(MetricsRole::BorderTextFocus);
                const BBox2f& g = getGeometry();
                const auto& render = _getRender();
                if (hasTextFocus())
                {
                    render->setFillColor(style->getColor(ColorRole::TextFocus));
                    drawBorder(render, g, btf);
                }
                const BBox2f g2 = g.margin(-btf);
                render->setFillColor(style->getColor(ColorRole::Border));
                drawBorder(render, g2, b);
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
                _redraw();
            }

            void KeyPressWidget::_textFocusLostEvent(Event::TextFocusLost& event)
            {
                event.accept();
                _redraw();
            }

            void KeyPressWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void KeyPressWidget::_updateEvent(Event::Update& event)
            {
                if (_textFocusInit)
                {
                    _textFocusInit = false;
                    takeTextFocus();
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
                void _init(const ShortcutDataPair&, const std::shared_ptr<Context>&);
                ShortcutWidget();

            public:
                virtual ~ShortcutWidget();

                static std::shared_ptr<ShortcutWidget> create(const ShortcutDataPair&, const std::shared_ptr<Context>&);

                void setCallback(const std::function<void(const ShortcutDataPair&)>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                ShortcutDataPair _shortcutPair;
                std::shared_ptr<KeyPressWidget> _keyPressWidget[2];
                std::shared_ptr<ToolButton> _clearButtons[2];
                std::shared_ptr<HorizontalLayout> _hLayouts[2];
                std::shared_ptr<FormLayout> _layout;
                std::function<void(const ShortcutDataPair&)> _callback;
            };

            void ShortcutWidget::_init(
                const ShortcutDataPair& shortcutPair,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::ShortcutWidget");

                _shortcutPair = shortcutPair;

                for (size_t i = 0; i < 2; ++i)
                {
                    _keyPressWidget[i] = KeyPressWidget::create(0 == i, context);

                    _clearButtons[i] = ToolButton::create(context);
                    _clearButtons[i]->setIcon("djvIconClear");
                    _clearButtons[i]->setInsideMargin(MetricsRole::None);
                }

                _layout = FormLayout::create(context);
                _layout->setMargin(MetricsRole::MarginSmall);
                for (size_t i = 0; i < 2; ++i)
                {
                    _hLayouts[i] = HorizontalLayout::create(context);
                    _hLayouts[i]->setSpacing(MetricsRole::None);
                    _hLayouts[i]->addChild(_keyPressWidget[i]);
                    _hLayouts[i]->setStretch(_keyPressWidget[i], RowStretch::Expand);
                    _hLayouts[i]->addChild(_clearButtons[i]);
                    _layout->addChild(_hLayouts[i]);
                }
                addChild(_layout);

                _widgetUpdate();

                auto weak = std::weak_ptr<ShortcutWidget>(std::dynamic_pointer_cast<ShortcutWidget>(shared_from_this()));
                _keyPressWidget[0]->setCallback(
                    [weak](const ShortcutData& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_callback)
                            {
                                widget->_shortcutPair.primary = value;
                                widget->_widgetUpdate();
                                if (widget->_callback)
                                {
                                    widget->_callback(widget->_shortcutPair);
                                }
                            }
                        }
                    });
                _keyPressWidget[1]->setCallback(
                    [weak](const ShortcutData& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_callback)
                            {
                                widget->_shortcutPair.secondary = value;
                                widget->_widgetUpdate();
                                if (widget->_callback)
                                {
                                    widget->_callback(widget->_shortcutPair);
                                }
                            }
                        }
                    });

                _clearButtons[0]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_callback)
                            {
                                widget->_shortcutPair.primary = ShortcutData();
                                widget->_widgetUpdate();
                                if (widget->_callback)
                                {
                                    widget->_callback(widget->_shortcutPair);
                                }
                            }
                        }
                    });

                _clearButtons[1]->setClickedCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_callback)
                            {
                                widget->_shortcutPair.secondary = ShortcutData();
                                widget->_widgetUpdate();
                                if (widget->_callback)
                                {
                                    widget->_callback(widget->_shortcutPair);
                                }
                            }
                        }
                    });
            }

            ShortcutWidget::ShortcutWidget()
            {}

            ShortcutWidget::~ShortcutWidget()
            {}

            std::shared_ptr<ShortcutWidget> ShortcutWidget::create(
                const ShortcutDataPair& shortcutPair,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ShortcutWidget>(new ShortcutWidget);
                out->_init(shortcutPair, context);
                return out;
            }

            void ShortcutWidget::setCallback(const std::function<void(const ShortcutDataPair&)>& value)
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
                    _layout->setText(_hLayouts[0], _getText("shortcut_primary") + ": ");
                    _layout->setText(_hLayouts[1], _getText("shortcut_secondary") + ": ");
                    for (size_t i = 0; i < 2; ++i)
                    {
                        _clearButtons[i]->setTooltip(_getText("shortcut_clear"));
                    }
                }
            }

            void ShortcutWidget::_widgetUpdate()
            {
                _keyPressWidget[0]->setShortcut(_shortcutPair.primary);
                _keyPressWidget[1]->setShortcut(_shortcutPair.secondary);
            }

        } // namespace

        struct ShortcutsWidget::Private
        {
            ShortcutDataMap shortcuts;
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
            const ShortcutDataPair& shortcutPair)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto shortcutWidget = ShortcutWidget::create(shortcutPair, context);

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
                    [weak, shortcut](const ShortcutDataPair& value)
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
                            if (widget->_p->window)
                            {
                                widget->_p->window->close();
                                widget->_p->window.reset();
                            }
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
                if (i.second.primary.isValid())
                {
                    taken[i.second.primary].insert(i.first);
                }
                if (i.second.secondary.isValid())
                {
                    taken[i.second.secondary].insert(i.first);
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
                    if (i.second.primary.isValid())
                    {
                        list.push_back(ShortcutData::getText(i.second.primary.key, i.second.primary.modifiers, textSystem));
                    }
                    if (i.second.secondary.isValid())
                    {
                        list.push_back(ShortcutData::getText(i.second.secondary.key, i.second.secondary.modifiers, textSystem));
                    }
                    const std::string shortcutText = String::join(list, ", ");
                    if (p.filter.empty() ||
                        String::match(text, p.filter) ||
                        String::match(shortcutText, p.filter))
                    {
                        auto button = ListButton::create(context);
                        button->setText(text);
                        button->setBackgroundRole(0 == index % 2 ? ColorRole::Trough : ColorRole::None);
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
                        const ShortcutDataPair shortcutPair = i.second;
                        button->setClickedCallback(
                            [weak, button, shortcut, shortcutPair]
                            {
                                if (auto widget = weak.lock())
                                {
                                    widget->_popup(button, shortcut, shortcutPair);
                                }
                            });

                        ++index;
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv

