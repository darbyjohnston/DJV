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

#include <djvViewApp/MediaWidgetPrivate.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        void PointerWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::ViewApp::MediaWidget::PointerWidget");
        }

        std::shared_ptr<PointerWidget> PointerWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PointerWidget>(new PointerWidget);
            out->_init(context);
            return out;
        }

        void PointerWidget::setHoverCallback(const std::function<void(PointerData)>& callback)
        {
            _hoverCallback = callback;
        }

        void PointerWidget::setDragCallback(const std::function<void(PointerData)>& callback)
        {
            _dragCallback = callback;
        }

        void PointerWidget::_pointerEnterEvent(Event::PointerEnter& event)
        {
            if (!event.isRejected())
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                if (_hoverCallback)
                {
                    _hoverCallback(PointerData(PointerState::Start, pos, std::map<int, bool>()));
                }
            }
        }

        void PointerWidget::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            if (_hoverCallback)
            {
                _hoverCallback(PointerData(PointerState::End, pos, std::map<int, bool>()));
            }
        }

        void PointerWidget::_pointerMoveEvent(Event::PointerMove& event)
        {
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            if (_pressedID)
            {
                if (_dragCallback)
                {
                    _dragCallback(PointerData(PointerState::Move, pos, _buttons));
                }
            }
            else
            {
                if (_hoverCallback)
                {
                    _hoverCallback(PointerData(PointerState::Move, pos, std::map<int, bool>()));
                }
            }
        }

        void PointerWidget::_buttonPressEvent(Event::ButtonPress& event)
        {
            if (_pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            _pressedID = info.id;
            _buttons = info.buttons;
            if (_dragCallback)
            {
                _dragCallback(PointerData(PointerState::Start, info.pos, info.buttons));
            }
        }

        void PointerWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            if (event.getPointerInfo().id != _pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            _pressedID = Event::InvalidID;
            _buttons = std::map<int, bool>();
            if (_dragCallback)
            {
                _dragCallback(PointerData(PointerState::End, info.pos, info.buttons));
            }
        }

    } // namespace ViewApp
} // namespace djv

