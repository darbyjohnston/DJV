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
#include <sstream>

namespace djv
{
    namespace Core
    {
        IEvent::IEvent(EventType eventType) :
            _eventType(eventType)
        {}

        IEvent::~IEvent()
        {}

        UpdateEvent::UpdateEvent(float dt) :
            IEvent(EventType::Update),
            _dt(dt)
        {}

        LocaleEvent::LocaleEvent(const std::string& locale) :
            IEvent(EventType::Locale),
            _locale(locale)
        {}

        IPointerEvent::IPointerEvent(const PointerInfo& pointerInfo, EventType eventType) :
            IEvent(eventType),
            _pointerInfo(pointerInfo)
        {}

        void IPointerEvent::setAccepted(bool value)
        {
            _accepted = value;
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

    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::,
        EventType,
        DJV_TEXT("Update"),
        DJV_TEXT("Locale"),
        DJV_TEXT("Pointer Enter"),
        DJV_TEXT("Pointer Leave"),
        DJV_TEXT("Pointer Move"),
        DJV_TEXT("Button Press"),
        DJV_TEXT("Button Release"));

} // namespace djv
