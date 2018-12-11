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

#include <djvCore/Event.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        IEvent::IEvent(EventType eventType) :
            _eventType(eventType)
        {}

        IEvent::~IEvent()
        {}

        void IEvent::setAccepted(bool value)
        {
            _accepted = value;
        }

        void IEvent::accept()
        {
            _accepted = true;
        }

        UpdateEvent::UpdateEvent(float dt) :
            IEvent(EventType::Update),
            _dt(dt)
        {}

        LocaleEvent::LocaleEvent(const std::string& locale) :
            IEvent(EventType::Locale),
            _locale(locale)
        {}

        PreLayoutEvent::PreLayoutEvent() :
            IEvent(EventType::PreLayout)
        {}

        LayoutEvent::LayoutEvent() :
            IEvent(EventType::Layout)
        {}

        ClipEvent::ClipEvent(const BBox2f& clipRect) :
            IEvent(EventType::Clip),
            _clipRect(clipRect)
        {}

        const BBox2f& ClipEvent::getClipRect() const
        {
            return _clipRect;
        }

        void ClipEvent::setClipRect(const BBox2f& value)
        {
            _clipRect = value;
        }

        PaintEvent::PaintEvent(const BBox2f& clipRect) :
            IEvent(EventType::Paint),
            _clipRect(clipRect)
        {}

        const BBox2f& PaintEvent::getClipRect() const
        {
            return _clipRect;
        }

        void PaintEvent::setClipRect(const BBox2f& value)
        {
            _clipRect = value;
        }

        IPointerEvent::IPointerEvent(const PointerInfo& pointerInfo, EventType eventType) :
            IEvent(eventType),
            _pointerInfo(pointerInfo)
        {}

        void IPointerEvent::setRejected(bool value)
        {
            _rejected = value;
        }

        void IPointerEvent::reject()
        {
            _rejected = true;
        }

        PointerEnterEvent::PointerEnterEvent(const PointerInfo& pointerInfo) :
            IPointerEvent(pointerInfo, EventType::PointerEnter)
        {}

        PointerLeaveEvent::PointerLeaveEvent(const PointerInfo& pointerInfo) :
            IPointerEvent(pointerInfo, EventType::PointerLeave)
        {}

        PointerMoveEvent::PointerMoveEvent(const PointerInfo& pointerInfo) :
            IPointerEvent(pointerInfo, EventType::PointerMove)
        {}

        ButtonPressEvent::ButtonPressEvent(const PointerInfo& pointerInfo) :
            IPointerEvent(pointerInfo, EventType::ButtonPress)
        {}

        ButtonReleaseEvent::ButtonReleaseEvent(const PointerInfo& pointerInfo) :
            IPointerEvent(pointerInfo, EventType::ButtonRelease)
        {}

        ScrollEvent::ScrollEvent(const glm::vec2& scrollDelta, const PointerInfo& info) :
            IPointerEvent(info, EventType::Scroll),
            _scrollDelta(scrollDelta)
        {}

        DropEvent::DropEvent(const std::vector<std::string>& dropPaths, const PointerInfo& info) :
            IPointerEvent(info, EventType::Drop),
            _dropPaths(dropPaths)
        {}

        KeyboardFocusEvent::KeyboardFocusEvent() :
            IEvent(EventType::KeyboardFocus)
        {}

        KeyboardFocusLostEvent::KeyboardFocusLostEvent() :
            IEvent(EventType::KeyboardFocusLost)
        {}

        KeyEvent::KeyEvent(int32_t keyCode, uint16_t keyModifiers, const PointerInfo& info) :
            IPointerEvent(info, EventType::Key),
            _keyCode(keyCode),
            _keyModifiers(keyModifiers)
        {}

        TextEvent::TextEvent(const std::string& text) :
            IEvent(EventType::Text),
            _text(text)
        {}

    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core,
        EventType,
        DJV_TEXT("Update"),
        DJV_TEXT("Locale"),
        DJV_TEXT("PreLayout"),
        DJV_TEXT("Layout"),
        DJV_TEXT("Clip"),
        DJV_TEXT("Paint"),
        DJV_TEXT("PointerEnter"),
        DJV_TEXT("PointerLeave"),
        DJV_TEXT("PointerMove"),
        DJV_TEXT("ButtonPress"),
        DJV_TEXT("ButtonRelease"),
        DJV_TEXT("Scroll"),
        DJV_TEXT("Drop"),
        DJV_TEXT("KeyboardFocus"),
        DJV_TEXT("KeyboardFocusLost"),
        DJV_TEXT("Key"),
        DJV_TEXT("Text"));

} // namespace djv
