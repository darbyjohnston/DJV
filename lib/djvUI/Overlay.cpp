// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Overlay.h>

#include <djvUI/Action.h>
#include <djvUI/StackLayout.h>

#include <djvSystem/Animation.h>
#include <djvSystem/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                //! \todo Should this be configurable?
                const size_t animationTime = 100;

            } // namespace

            struct Overlay::Private
            {
                bool capturePointer = true;
                bool captureKeyboard = true;
                bool fadeIn = true;
                bool closeOnEscape = true;
                std::shared_ptr<System::Animation::Animation> fadeAnimation;
                std::map<System::Event::PointerID, bool> pressedIDs;
                std::function<void(void)> closeCallback;
            };

            void Overlay::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Overlay");
                setBackgroundColorRole(ColorRole::Overlay);
                setPointerEnabled(true);

                p.fadeAnimation = System::Animation::Animation::create(context);
            }

            Overlay::Overlay() :
                _p(new Private)
            {}

            Overlay::~Overlay()
            {}

            std::shared_ptr<Overlay> Overlay::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Overlay>(new Overlay);
                out->_init(context);
                return out;
            }

            bool Overlay::hasCapturePointer() const
            {
                return _p->capturePointer;
            }

            bool Overlay::hasCaptureKeyboard() const
            {
                return _p->captureKeyboard;
            }

            void Overlay::setCapturePointer(bool value)
            {
                _p->capturePointer = value;
                setPointerEnabled(value);
            }

            void Overlay::setCaptureKeyboard(bool value)
            {
                _p->captureKeyboard = value;
            }

            bool Overlay::hasFadeIn() const
            {
                return _p->fadeIn;
            }

            void Overlay::setFadeIn(bool value)
            {
                _p->fadeIn = value;
            }

            void Overlay::setCloseCallback(const std::function<void(void)>& callback)
            {
                _p->closeCallback = callback;
            }

            void Overlay::setVisible(bool value)
            {
                Widget::setVisible(value);
                DJV_PRIVATE_PTR();
                if (p.fadeIn && p.fadeAnimation)
                {
                    setOpacity(0.F);
                    if (value)
                    {
                        auto weak = std::weak_ptr<Overlay>(std::dynamic_pointer_cast<Overlay>(shared_from_this()));
                        p.fadeAnimation->start(
                            getOpacity(),
                            1.F,
                            std::chrono::milliseconds(animationTime),
                            [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setOpacity(value);
                            }
                        },
                            [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->setOpacity(value);
                            }
                        });
                    }
                }
                else
                {
                    setOpacity(1.F);
                }
            }

            float Overlay::getHeightForWidth(float value) const
            {
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                float out = stackHeightForWidth(value - m.x, getChildWidgets(), Margin(), style) + m.y;
                return out;
            }

            void Overlay::_preLayoutEvent(System::Event::PreLayout& event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(stackMinimumSize(getChildWidgets(), Margin(), style) + getMargin().getSize(style));
            }

            void Overlay::_layoutEvent(System::Event::Layout& event)
            {
                const auto& style = _getStyle();
                stackLayout(getMargin().bbox(getGeometry(), style), getChildWidgets(), Margin(), style);
            }

            void Overlay::_pointerEnterEvent(System::Event::PointerEnter& event)
            {
                if (_p->capturePointer && !event.isRejected())
                {
                    event.accept();
                }
            }

            void Overlay::_pointerLeaveEvent(System::Event::PointerLeave& event)
            {
                if (_p->capturePointer)
                {
                    event.accept();
                }
            }

            void Overlay::_pointerMoveEvent(System::Event::PointerMove& event)
            {
                if (_p->capturePointer)
                {
                    event.accept();
                }
            }

            void Overlay::_buttonPressEvent(System::Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.capturePointer)
                {
                    event.accept();
                    p.pressedIDs[event.getPointerInfo().id] = true;
                    _doCloseCallback();
                }
            }

            void Overlay::_buttonReleaseEvent(System::Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.pressedIDs.find(event.getPointerInfo().id);
                if (i != p.pressedIDs.end())
                {
                    event.accept();
                    p.pressedIDs.erase(i);
                }
            }

            void Overlay::_keyPressEvent(System::Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                DJV_PRIVATE_PTR();
                if (!event.isAccepted())
                {
                    if (p.closeOnEscape && GLFW_KEY_ESCAPE == event.getKey())
                    {
                        event.accept();
                        _doCloseCallback();
                    }
                    else if (p.captureKeyboard)
                    {
                        event.accept();
                    }
                }
            }

            void Overlay::_doCloseCallback()
            {
                DJV_PRIVATE_PTR();
                if (p.closeCallback)
                {
                    p.closeCallback();
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
