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

#include <djvCore/BBox.h>
#include <djvCore/Enum.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace djv
{
    namespace Core
    {
        class IObject;

        //! This namespace provides event system functionality.
        namespace Event
        {
            //! This enumeration provides the event types.
            enum class Type
            {
                Update,
                Locale,
                PreLayout,
                Layout,
                Clip,
                Paint,
                PointerEnter,
                PointerLeave,
                PointerMove,
                ButtonPress,
                ButtonRelease,
                Scroll,
                Drop,
                KeyboardFocus,
                KeyboardFocusLost,
                KeyPress,
                KeyRelease,
                Text,

                Count,
                First = Update
            };
            DJV_ENUM_HELPERS(Type);

            //! This class provides the interface for events.
            class IEvent
            {
            protected:
                IEvent(Type);

            public:
                virtual ~IEvent() = 0;

                Type getEventType() const { return _eventType; }

                bool isAccepted() const { return _accepted; }
                void setAccepted(bool);
                void accept();

            private:
                Type _eventType;
                bool _accepted = false;
            };

            //! This class provides an update event.
            class Update : public IEvent
            {
            public:
                Update(float dt);

                float getDeltaTime() const { return _dt; }

            private:
                float _dt = 0.f;
            };

            //! This class provides a locale event.
            class Locale : public IEvent
            {
            public:
                Locale(const std::string&);

                const std::string& getLocale() const { return _locale; }

            private:
                std::string _locale;
            };

            //! This class provides an event to prepare for layout.
            class PreLayout : public IEvent
            {
            public:
                PreLayout();
            };

            class Layout : public IEvent
            {
            public:
                Layout();
            };

            //! This class provides a clip event.
            class Clip : public IEvent
            {
            public:
                Clip(const BBox2f& clipRect);

                const BBox2f& getClipRect() const;
                void setClipRect(const BBox2f&);

            private:
                BBox2f _clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
            };

            //! This class provides a paint event.
            class Paint : public IEvent
            {
            public:
                Paint(const BBox2f& clipRect);

                const BBox2f& getClipRect() const;
                void setClipRect(const BBox2f&);

            private:
                BBox2f _clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
            };

            //! This typedef provides a pointer ID.
            typedef uint32_t PointerID;

            //! This struct provides information about the pointer.
            struct PointerInfo
            {
                PointerID id = 0;
                glm::vec3 pos;
                glm::vec3 dir;
                glm::vec2 projectedPos;
                int button = 0;
            };

            //! This class provides the interface for pointer events.
            class IPointer : public IEvent
            {
            protected:
                IPointer(const PointerInfo&, Type);

            public:
                ~IPointer() override = 0;

                bool isRejected() const { return _rejected; }
                void setRejected(bool);
                void reject();

                const PointerInfo& getPointerInfo() const { return _pointerInfo; }

            private:
                bool _rejected = false;
                PointerInfo _pointerInfo;
            };

            //! This class provides a pointer enter event.
            class PointerEnter : public IPointer
            {
            public:
                PointerEnter(const PointerInfo&);
            };

            //! This class provides a pointer leave event.
            class PointerLeave : public IPointer
            {
            public:
                PointerLeave(const PointerInfo&);
            };

            //! This class provides a pointer move event.
            class PointerMove : public IPointer
            {
            public:
                PointerMove(const PointerInfo&);
            };

            //! This class provides a button press event.
            class ButtonPress : public IPointer
            {
            public:
                ButtonPress(const PointerInfo&);
            };

            //! This class provides a button release event.
            class ButtonRelease : public IPointer
            {
            public:
                ButtonRelease(const PointerInfo&);
            };

            //! This class provides a scroll event.
            class Scroll : public IPointer
            {
            public:
                Scroll(const glm::vec2& scrollDelta, const PointerInfo&);

                const glm::vec2& getScrollDelta() const { return _scrollDelta; }

            private:
                glm::vec2 _scrollDelta = glm::vec2(0.f, 0.f);
            };

            //! This class provides a drag and drop event.
            class Drop : public IPointer
            {
            public:
                Drop(const std::vector<std::string>&, const PointerInfo&);

                const std::vector<std::string>& getDropPaths() const { return _dropPaths; }

            private:
                std::vector<std::string> _dropPaths;
            };

            //! This class provides a keyboard focus event.
            class KeyboardFocus : public IEvent
            {
            public:
                KeyboardFocus();
            };

            //! This class provides a keyboard focus lost event.
            class KeyboardFocusLost : public IEvent
            {
            public:
                KeyboardFocusLost();
            };

            //! This class provides the interface for key events.
            class IKey : public IPointer
            {
            protected:
                IKey(int key, int keyModifiers, const PointerInfo&, Type);

            public:
                ~IKey() override = 0;

                int getKey() const { return _key; }
                int getKeyModifiers() const { return _keyModifiers; }

            private:
                int _key = 0;
                int _keyModifiers = 0;
            };

            //! This class provides a key press event.
            class KeyPress : public IKey
            {
            public:
                KeyPress(int key, int keyModifiers, const PointerInfo&);
            };

            //! This class provides a key release event.
            class KeyRelease : public IKey
            {
            public:
                KeyRelease(int key, int keyModifiers, const PointerInfo&);
            };

            //! This class provides a text entry event.
            class Text : public IEvent
            {
            public:
                Text(const std::string&);

                const std::string& getText() const { return _text; }

            private:
                std::string _text;
            };

        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Event::Type);

} // namespace djv
