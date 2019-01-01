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

#include <djvUI/Overlay.h>

#include <djvUI/Action.h>
#include <djvUI/Shortcut.h>
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
                bool fadeIn = true;
                std::shared_ptr<Stack> layout;
                std::shared_ptr<Animation::Animation> fadeAnimation;
                std::function<void(void)> closeCallback;
            };

            void Overlay::_init(Context * context)
            {
                IContainer::_init(context);

                setClassName("djv::UI::Layout::Overlay");
                setBackgroundRole(Style::ColorRole::Overlay);
                setPointerEnabled(_p->capturePointer);
                setVisible(false);

                auto closeShortcut = Shortcut::create(GLFW_KEY_ESCAPE);
                auto closeAction = Action::create();
                closeAction->setShortcut(closeShortcut);
                addAction(closeAction);

                _p->layout = Stack::create(context);
                IContainer::addWidget(_p->layout);

                _p->fadeAnimation = Animation::Animation::create(context);

                auto weak = std::weak_ptr<Overlay>(std::dynamic_pointer_cast<Overlay>(shared_from_this()));
                closeShortcut->setCallback(
                    [weak]
                {
                    if (auto overlay = weak.lock())
                    {
                        if (overlay->_p->closeCallback)
                        {
                            overlay->_p->closeCallback();
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

            void Overlay::addWidget(const std::shared_ptr<Widget>& value)
            {
                _p->layout->addWidget(value);
            }

            void Overlay::removeWidget(const std::shared_ptr<Widget>& value)
            {
                _p->layout->removeWidget(value);
            }

            void Overlay::clearWidgets()
            {
                _p->layout->clearWidgets();
            }

            void Overlay::setVisible(bool value)
            {
                IContainer::setVisible(value);
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
                return _p->layout->getHeightForWidth(value);
            }

            void Overlay::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Overlay::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Overlay::_buttonPressEvent(Event::ButtonPress& event)
            {
                if (_p->capturePointer)
                {
                    event.accept();
                    if (_p->closeCallback)
                    {
                        _p->closeCallback();
                    }
                }
            }

            void Overlay::_keyPressEvent(Event::KeyPress& event)
            {
                Widget::_keyPressEvent(event);
                if (_p->captureKeyboard)
                {
                    event.accept();
                }
            }

            void Overlay::_keyReleaseEvent(Event::KeyRelease& event)
            {
                if (_p->captureKeyboard)
                {
                    event.accept();
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
