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

#include <map>

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
                Parent,
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
                inline IEvent(Type);

            public:
                inline virtual ~IEvent() = 0;

                inline Type getEventType() const;

                inline bool isAccepted() const;
                inline void setAccepted(bool);
                inline void accept();

            private:
                Type _eventType;
                bool _accepted = false;
            };

            //! This class provides an event for when an object's parent changes.
            class Parent : public IEvent
            {
            public:
                inline Parent(const std::weak_ptr<IObject> & prevParent, const std::weak_ptr<IObject> & newParent);

                inline const std::weak_ptr<IObject> & getPrevParent() const;
                inline const std::weak_ptr<IObject> & getNewParent() const;

            private:
                std::weak_ptr<IObject> _prevParent;
                std::weak_ptr<IObject> _newParent;
            };

            //! This class provides an update event.
            class Update : public IEvent
            {
            public:
                inline Update(float dt);

                inline float getDeltaTime() const;

            private:
                float _dt;
            };

            //! This class provides a locale event.
            class Locale : public IEvent
            {
            public:
                inline Locale(const std::string&);

                inline const std::string& getLocale() const;

            private:
                std::string _locale;
            };

            //! This class provides an event to prepare for user interface layout.
            class PreLayout : public IEvent
            {
            public:
                inline PreLayout();
            };

            //! This class provides an event for user interface layout.
            class Layout : public IEvent
            {
            public:
                inline Layout();
            };

            //! This class provides a clip event.
            class Clip : public IEvent
            {
            public:
                inline Clip(const BBox2f& clipRect);

                inline const BBox2f& getClipRect() const;
                inline void setClipRect(const BBox2f&);

            private:
                BBox2f _clipRect;
            };

            //! This class provides a paint event.
            class Paint : public IEvent
            {
            public:
                inline Paint(const BBox2f& clipRect);

                inline const BBox2f& getClipRect() const;
                inline void setClipRect(const BBox2f&);

            private:
                BBox2f _clipRect;
            };

            //! This typedef provides a pointer ID.
            typedef uint32_t PointerID;

            //! This struct provides information about the pointer.
            struct PointerInfo
            {
                PointerID id = 0;
                glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f);
                glm::vec3 dir = glm::vec3(0.f, 0.f, 0.f);
                glm::vec2 projectedPos = glm::vec2(-1.f, -1.f);
                std::map<int, bool> buttons;
            };

            //! This class provides the interface for pointer events.
            class IPointer : public IEvent
            {
            protected:
                inline IPointer(const PointerInfo&, Type);

            public:
                inline ~IPointer() override = 0;

                inline bool isRejected() const;
                inline void setRejected(bool);
                inline void reject();

                inline const PointerInfo& getPointerInfo() const;

            private:
                bool _rejected = false;
                PointerInfo _pointerInfo;
            };

            //! This class provides a pointer enter event.
            class PointerEnter : public IPointer
            {
            public:
                inline PointerEnter(const PointerInfo&);
            };

            //! This class provides a pointer leave event.
            class PointerLeave : public IPointer
            {
            public:
                inline PointerLeave(const PointerInfo&);
            };

            //! This class provides a pointer move event.
            class PointerMove : public IPointer
            {
            public:
                inline PointerMove(const PointerInfo&);
            };

            //! This class provides a button press event.
            class ButtonPress : public IPointer
            {
            public:
                inline ButtonPress(const PointerInfo&);
            };

            //! This class provides a button release event.
            class ButtonRelease : public IPointer
            {
            public:
                inline ButtonRelease(const PointerInfo&);
            };

            //! This class provides a scroll event.
            class Scroll : public IPointer
            {
            public:
                inline Scroll(const glm::vec2& scrollDelta, const PointerInfo&);

                inline const glm::vec2& getScrollDelta() const;

            private:
                glm::vec2 _scrollDelta;
            };

            //! This class provides a drag and drop event.
            class Drop : public IPointer
            {
            public:
                inline Drop(const std::vector<std::string>&, const PointerInfo&);

                inline const std::vector<std::string>& getDropPaths() const;

            private:
                std::vector<std::string> _dropPaths;
            };

            //! This class provides a keyboard focus event.
            class KeyboardFocus : public IEvent
            {
            public:
                inline KeyboardFocus();
            };

            //! This class provides a keyboard focus lost event.
            class KeyboardFocusLost : public IEvent
            {
            public:
                inline KeyboardFocusLost();
            };

            //! This class provides the interface for key events.
            class IKey : public IPointer
            {
            protected:
                inline IKey(int key, int keyModifiers, const PointerInfo&, Type);

            public:
                inline ~IKey() override = 0;

                inline int getKey() const;
                inline int getKeyModifiers() const;

            private:
                int _key;
                int _keyModifiers;
            };

            //! This class provides a key press event.
            class KeyPress : public IKey
            {
            public:
                inline KeyPress(int key, int keyModifiers, const PointerInfo&);
            };

            //! This class provides a key release event.
            class KeyRelease : public IKey
            {
            public:
                inline KeyRelease(int key, int keyModifiers, const PointerInfo&);
            };

            //! This class provides a text entry event.
            class Text : public IEvent
            {
            public:
                inline Text(const std::string&);

                inline const std::string& getText() const;

            private:
                std::string _text;
            };

        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Event::Type);

} // namespace djv

#include <djvCore/EventInline.h>
