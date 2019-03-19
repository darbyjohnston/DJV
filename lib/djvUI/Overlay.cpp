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

#include <djvUI/Overlay.h>

#include <djvUI/Action.h>
#include <djvUI/StackLayout.h>

#include <djvCore/Animation.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Overlay::Private
            {
                bool capturePointer = true;
                bool captureKeyboard = true;
                std::weak_ptr<Widget> anchor;
                bool fadeIn = true;
                std::shared_ptr<Stack> layout;
                std::shared_ptr<Animation::Animation> fadeAnimation;
                std::map<Event::PointerID, bool> pressedIDs;
                bool keyPress = false;
                std::function<void(void)> closeCallback;
                std::shared_ptr<ValueObserver<bool> > closeObserver;
            };

            void Overlay::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Overlay");
                setBackgroundRole(ColorRole::Overlay);
                setVisible(false);

                auto closeAction = Action::create();
                closeAction->setShortcut(GLFW_KEY_ESCAPE);
                addAction(closeAction);

                DJV_PRIVATE_PTR();
                p.layout = Stack::create(context);
                Widget::addChild(p.layout);

                p.fadeAnimation = Animation::Animation::create(context);

                auto weak = std::weak_ptr<Overlay>(std::dynamic_pointer_cast<Overlay>(shared_from_this()));
                p.closeObserver = ValueObserver<bool>::create(
                    closeAction->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto overlay = weak.lock())
                        {
                            if (overlay->_p->closeCallback)
                            {
                                overlay->_p->closeCallback();
                            }
                        }
                    }
                });
            }

            Overlay::Overlay() :
                _p(new Private)
            {}

            Overlay::~Overlay()
            {}

            std::shared_ptr<Overlay> Overlay::create(Context * context)
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

            const std::weak_ptr<Widget> & Overlay::getAnchor() const
            {
                return _p->anchor;
            }

            void Overlay::setAnchor(const std::weak_ptr<Widget> & value)
            {
                _p->anchor = value;
            }

            bool Overlay::hasFadeIn() const
            {
                return _p->fadeIn;
            }

            void Overlay::setFadeIn(bool value)
            {
                _p->fadeIn = value;
            }

            void Overlay::setCloseCallback(const std::function<void(void)> & callback)
            {
                _p->closeCallback = callback;
            }

            void Overlay::setVisible(bool value)
            {
                Widget::setVisible(value);
                if (_p->fadeIn && _p->fadeAnimation)
                {
                    setOpacity(0.f);
                    if (value)
                    {
                        auto weak = std::weak_ptr<Overlay>(std::dynamic_pointer_cast<Overlay>(shared_from_this()));
                        _p->fadeAnimation->start(
                            getOpacity(),
                            1.f,
                            std::chrono::milliseconds(100),
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
                    setOpacity(1.f);
                }
            }

            float Overlay::getHeightForWidth(float value) const
            {
                float out = 0.f;
                if (auto style = _getStyle().lock())
                {
                    const glm::vec2 m = getMargin().getSize(style);
                    out = _p->layout->getHeightForWidth(value - m.x) + m.y;
                }
                return out;
            }

            void Overlay::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->layout->addChild(value);
            }

            void Overlay::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->layout->removeChild(value);
            }

            void Overlay::_preLayoutEvent(Event::PreLayout & event)
            {
                if (auto style = _getStyle().lock())
                {
                    _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
                }
            }

            void Overlay::_layoutEvent(Event::Layout & event)
            {
                if (auto style = _getStyle().lock())
                {
                    _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
                }
            }

            void Overlay::_pointerEnterEvent(Core::Event::PointerEnter & event)
            {
                if (_p->capturePointer && !event.isRejected() && !_isInsideAnchor(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                }
            }

            void Overlay::_pointerLeaveEvent(Core::Event::PointerLeave & event)
            {
                if (_p->capturePointer && !_isInsideAnchor(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                }
            }

            void Overlay::_pointerMoveEvent(Core::Event::PointerMove & event)
            {
                if (_p->capturePointer && !_isInsideAnchor(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                }
            }

            void Overlay::_buttonPressEvent(Event::ButtonPress & event)
            {
                DJV_PRIVATE_PTR();
                if (p.capturePointer && !_isInsideAnchor(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                    p.pressedIDs[event.getPointerInfo().id] = true;
                    if (p.closeCallback)
                    {
                        p.closeCallback();
                    }
                }
            }

            void Overlay::_buttonReleaseEvent(Core::Event::ButtonRelease & event)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.pressedIDs.find(event.getPointerInfo().id);
                if (i != p.pressedIDs.end())
                {
                    event.accept();
                    p.pressedIDs.erase(i);
                }
            }

            void Overlay::_keyPressEvent(Event::KeyPress & event)
            {
                Widget::_keyPressEvent(event);
                DJV_PRIVATE_PTR();
                if (p.captureKeyboard && !_isInsideAnchor(event.getPointerInfo().projectedPos))
                {
                    event.accept();
                    p.keyPress = true;
                }
            }

            void Overlay::_keyReleaseEvent(Event::KeyRelease & event)
            {
                DJV_PRIVATE_PTR();
                if (p.keyPress)
                {
                    event.accept();
                    p.keyPress = false;
                }
            }

            bool Overlay::_isInsideAnchor(const glm::vec2 & pos) const
            {
                bool out = false;
                if (auto anchor = _p->anchor.lock())
                {
                    out = anchor->getGeometry().contains(pos);
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
