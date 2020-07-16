// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidgetPrivate.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PointerWidget::Private
        {
            uint32_t pressedID = Core::Event::invalidID;
            std::map<int, bool> buttons;
            int key = 0;
            int keyModifiers = 0;
            std::function<void(const PointerData&)> hoverCallback;
            std::function<void(const PointerData&)> dragCallback;
            std::function<void(const ScrollData&)> scrollCallback;
        };

        void PointerWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::ViewApp::PointerWidget");
        }

        PointerWidget::PointerWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PointerWidget> PointerWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PointerWidget>(new PointerWidget);
            out->_init(context);
            return out;
        }

        void PointerWidget::setHoverCallback(const std::function<void(const PointerData&)>& callback)
        {
            _p->hoverCallback = callback;
        }

        void PointerWidget::setDragCallback(const std::function<void(const PointerData&)>& callback)
        {
            _p->dragCallback = callback;
        }

        void PointerWidget::setScrollCallback(const std::function<void(const ScrollData&)>& callback)
        {
            _p->scrollCallback = callback;
        }

        void PointerWidget::_pointerEnterEvent(Event::PointerEnter& event)
        {
            DJV_PRIVATE_PTR();
            if (!event.isRejected())
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                _doHoverCallback(PointerData(PointerState::Start, pos, std::map<int, bool>(), p.key, p.keyModifiers));
            }
        }

        void PointerWidget::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            _doHoverCallback(PointerData(PointerState::End, pos, std::map<int, bool>(), p.key, p.keyModifiers));
        }

        void PointerWidget::_pointerMoveEvent(Event::PointerMove& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            if (p.pressedID)
            {
                _doDragCallback(PointerData(PointerState::Move, pos, p.buttons, p.key, p.keyModifiers));
            }
            else
            {
                _doHoverCallback(PointerData(PointerState::Move, pos, std::map<int, bool>(), p.key, p.keyModifiers));
            }
        }

        void PointerWidget::_buttonPressEvent(Event::ButtonPress& event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            p.pressedID = info.id;
            p.buttons = info.buttons;
            _doDragCallback(PointerData(PointerState::Start, info.pos, info.buttons, p.key, p.keyModifiers));
        }

        void PointerWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getPointerInfo().id != p.pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            p.pressedID = Event::invalidID;
            p.buttons = std::map<int, bool>();
            _doDragCallback(PointerData(PointerState::End, info.pos, info.buttons, p.key, p.keyModifiers));
        }

        void PointerWidget::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            if (!event.isAccepted())
            {
                const int key = event.getKey();
                const int keyModifiers = event.getKeyModifiers();
                if ((GLFW_KEY_LEFT_CONTROL == key || GLFW_KEY_RIGHT_CONTROL == key) &&
                    keyModifiers & GLFW_MOD_CONTROL)
                {
                    event.accept();
                    p.key = event.getKey();
                    p.keyModifiers = event.getKeyModifiers();
                }
                else
                {
                    p.key = 0;
                    p.keyModifiers = 0;
                }
            }
        }

        void PointerWidget::_keyReleaseEvent(Event::KeyRelease& event)
        {
            DJV_PRIVATE_PTR();
            p.key = 0;
            p.keyModifiers = 0;
        }

        void PointerWidget::_scrollEvent(Event::Scroll& event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            if (p.scrollCallback)
            {
                p.scrollCallback(ScrollData(event.getScrollDelta(), p.key, p.keyModifiers));
            }
        }

        void PointerWidget::_doHoverCallback(const PointerData& value)
        {
            DJV_PRIVATE_PTR();
            if (p.hoverCallback)
            {
                p.hoverCallback(value);
            }
        }

        void PointerWidget::_doDragCallback(const PointerData& value)
        {
            DJV_PRIVATE_PTR();
            if (p.dragCallback)
            {
                p.dragCallback(value);
            }
        }

    } // namespace ViewApp
} // namespace djv

