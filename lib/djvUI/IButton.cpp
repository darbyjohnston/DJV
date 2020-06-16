// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IButton.h>

#include <djvUI/Action.h>
#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct IButton::Private
            {
                ButtonType buttonType = ButtonType::Push;
                bool checked = false;
                ColorRole foregroundColorRole = ColorRole::Foreground;
                std::shared_ptr<Action> action;

                std::function<void(void)> clickedCallback;
                std::function<void(bool)> checkedCallback;

                Event::PointerID pressedID = Event::invalidID;
                glm::vec2 pressedPos = glm::vec2(0.F, 0.F);
                bool canRejectPressed = true;

                std::shared_ptr<ValueObserver<ButtonType> > buttonTypeObserver;
                std::shared_ptr<ValueObserver<bool> > checkedObserver;
                std::shared_ptr<ValueObserver<bool> > enabledObserver;
                std::shared_ptr<ValueObserver<std::string> > tooltipObserver;
            };

            IButton::IButton() :
                _p(new Private)
            {
                setClassName("djv::UI::Button::IButton");
                setPointerEnabled(true);
            }

            IButton::~IButton()
            {}

            ButtonType IButton::getButtonType() const
            {
                return _p->buttonType;
            }

            void IButton::setButtonType(ButtonType value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.buttonType)
                    return;
                p.buttonType = value;
                _resize();
            }

            bool IButton::isChecked() const
            {
                return _p->checked;
            }

            void IButton::setChecked(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.checked)
                    return;
                p.checked = value;
                _redraw();
            }

            ColorRole IButton::getForegroundColorRole() const
            {
                return _p->foregroundColorRole;
            }

            void IButton::setForegroundColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.foregroundColorRole)
                    return;
                p.foregroundColorRole = value;
                _redraw();
            }

            void IButton::setClickedCallback(const std::function<void(void)>& callback)
            {
                _p->clickedCallback = callback;
            }

            void IButton::setCheckedCallback(const std::function<void(bool)>& callback)
            {
                _p->checkedCallback = callback;
            }

            void IButton::addAction(const std::shared_ptr<Action>& value)
            {
                Widget::addAction(value);
                _actionUpdate();
            }

            void IButton::removeAction(const std::shared_ptr<Action>& value)
            {
                Widget::removeAction(value);
                _actionUpdate();
            }

            void IButton::clearActions()
            {
                Widget::clearActions();
                _actionUpdate();
            }

            void IButton::_pointerEnterEvent(Event::PointerEnter& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void IButton::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                event.accept();
                if (isEnabled(true))
                {
                    _redraw();
                }
            }

            void IButton::_pointerMoveEvent(Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                if (id == p.pressedID)
                {
                    const float distance = glm::length(pos - p.pressedPos);
                    const auto& style = _getStyle();
                    const bool accepted = p.canRejectPressed ? distance < style->getMetric(MetricsRole::Drag) : true;
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        p.pressedID = Event::invalidID;
                        _redraw();
                    }
                }
            }

            void IButton::_buttonPressEvent(Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID)
                    return;
                event.accept();
                const auto& pointerInfo = event.getPointerInfo();
                p.pressedID  = pointerInfo.id;
                p.pressedPos = pointerInfo.projectedPos;
                _redraw();
            }

            void IButton::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto& pointerInfo = event.getPointerInfo();
                if (pointerInfo.id == p.pressedID)
                {
                    event.accept();
                    p.pressedID = Event::invalidID;
                    const BBox2f& g = getGeometry();
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (i != hover.end() && g.contains(i->second))
                    {
                        _doClickedCallback();
                        switch (p.buttonType)
                        {
                        case ButtonType::Toggle:
                            setChecked(!p.checked);
                            _doCheckedCallback(p.checked);
                            break;
                        case ButtonType::Radio:
                            if (!p.checked)
                            {
                                setChecked(true);
                                _doCheckedCallback(p.checked);
                            }
                            break;
                        case ButtonType::Exclusive:
                            setChecked(!p.checked);
                            _doCheckedCallback(p.checked);
                            break;
                        default: break;
                        }
                        _redraw();
                    }
                }
            }

            bool IButton::_isToggled() const
            {
                DJV_PRIVATE_PTR();
                bool out = false;
                if (p.pressedID)
                {
                    const auto& hover = _getPointerHover();
                    const auto i = hover.find(p.pressedID);
                    if (i != hover.end())
                    {
                        const BBox2f& g = getGeometry();
                        switch (p.buttonType)
                        {
                        case ButtonType::Toggle:
                            out = g.contains(i->second) ? !p.checked : p.checked;
                            break;
                        case ButtonType::Radio:
                            if (p.checked)
                            {
                                out = p.checked;
                            }
                            else
                            {
                                out = g.contains(i->second) ? !p.checked : p.checked;
                            }
                            break;
                        case ButtonType::Exclusive:
                            out = g.contains(i->second) ? !p.checked : p.checked;
                            break;
                        default: break;
                        }
                    }
                }
                else
                {
                    out = p.checked;
                }
                return out;
            }

            bool IButton::_isHovered() const
            {
                return isEnabled(true) && _getPointerHover().size();
            }

            bool IButton::_isPressed() const
            {
                return _p->pressedID != 0;
            }

            const glm::vec2& IButton::_getPressedPos() const
            {
                return _p->pressedPos;
            }

            void IButton::_setCanRejectPressed(bool value)
            {
                _p->canRejectPressed = value;
            }

            void IButton::_doClickedCallback()
            {
                DJV_PRIVATE_PTR();
                if (p.clickedCallback)
                {
                    p.clickedCallback();
                }
            }

            void IButton::_doCheckedCallback(bool value)
            {
                DJV_PRIVATE_PTR();
                if (p.checkedCallback)
                {
                    p.checkedCallback(value);
                }
            }

            void IButton::_actionUpdate()
            {
                DJV_PRIVATE_PTR();
                const auto& actions = getActions();
                if (!actions.empty())
                {
                    p.action = actions.front();
                    auto weak = std::weak_ptr<IButton>(std::dynamic_pointer_cast<IButton>(shared_from_this()));
                    p.buttonTypeObserver = ValueObserver<ButtonType>::create(
                        p.action->observeButtonType(),
                        [weak](ButtonType value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setButtonType(value);
                            }
                        });
                    p.checkedObserver = ValueObserver<bool>::create(
                        p.action->observeChecked(),
                        [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setChecked(value);
                            }
                        });
                    p.enabledObserver = ValueObserver<bool>::create(
                        p.action->observeEnabled(),
                        [weak](bool value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setEnabled(value);
                            }
                        });
                    p.tooltipObserver = ValueObserver<std::string>::create(
                        p.action->observeTooltip(),
                        [weak](const std::string& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setTooltip(value);
                            }
                        });
                }
                else
                {
                    p.action.reset();
                    p.buttonTypeObserver.reset();
                    p.checkedObserver.reset();
                    p.enabledObserver.reset();
                    p.tooltipObserver.reset();
                }
            }


        } // namespace Button
    } // namespace UI
} // namespace djv

