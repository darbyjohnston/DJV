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

#pragma once

#include <djvCore/Enum.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace djv
{
    namespace Core
    {
        class IObject;

        //! This enumeration provides the event types.
        enum class EventType
        {
            Update,
            Locale,
            PointerEnter,
            PointerLeave,
            PointerMove,
            ButtonPress,
            ButtonRelease,

            Count,
            First = Update
        };
        DJV_ENUM_HELPERS(EventType);

        class IEvent
        {
        protected:
            IEvent(EventType);

        public:
            virtual ~IEvent() = 0;

            EventType getEventType() const { return _eventType; }

        private:
            EventType _eventType;
        };

        class UpdateEvent : public IEvent
        {
        public:
            UpdateEvent(float dt);

            float getDeltaTime() const { return _dt; }

        private:
            float _dt = 0.f;
        };

        class LocaleEvent : public IEvent
        {
        public:
            LocaleEvent(const std::string&);

            const std::string& getLocale() const { return _locale; }

        private:
            std::string _locale;
        };

        typedef uint32_t PointerID;

        struct PointerInfo
        {
            PointerID id = 0;
            glm::vec3 pos;
            glm::vec3 dir;
            glm::vec2 projectedPos;
            int button = 0;
        };

        class IPointerEvent : public IEvent
        {
        public:
            IPointerEvent(const PointerInfo&, EventType);

            bool isAccepted() const { return _accepted; }
            void setAccepted(bool);

            const PointerInfo& getPointerInfo() const { return _pointerInfo; }

        private:
            bool _accepted = false;
            PointerInfo _pointerInfo;
        };

        class PointerEnterEvent : public IPointerEvent
        {
        public:
            PointerEnterEvent(const PointerInfo&);
        };

        class PointerLeaveEvent : public IPointerEvent
        {
        public:
            PointerLeaveEvent(const PointerInfo&);
        };

        class PointerMoveEvent : public IPointerEvent
        {
        public:
            PointerMoveEvent(const PointerInfo&);
        };

        class ButtonPressEvent : public IPointerEvent
        {
        public:
            ButtonPressEvent(const PointerInfo&);
        };

        class ButtonReleaseEvent : public IPointerEvent
        {
        public:
            ButtonReleaseEvent(const PointerInfo&);
        };

    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::EventType);

} // namespace djv
