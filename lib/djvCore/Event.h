// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/BBox.h>
#include <djvCore/Enum.h>
#include <djvCore/Time.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <chrono>
#include <map>
#include <memory>

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
                ParentChanged,
                ChildAdded,
                ChildRemoved,
                ChildOrder,
                Init,
                Update,
                InitLayout,
                PreLayout,
                Layout,
                Clip,
                Paint,
                PaintOverlay,
                PointerEnter,
                PointerLeave,
                PointerMove,
                ButtonPress,
                ButtonRelease,
                Scroll,
                Drop,
                KeyPress,
                KeyRelease,
                TextFocus,
                TextFocusLost,
                TextInput,

                Count,
                First = Update
            };
            DJV_ENUM_HELPERS(Type);

            //! This class provides the event base class.
            class Event
            {
            protected:
                explicit Event(Type);

            public:
                virtual ~Event() = 0;

                Type getEventType() const;

                bool isAccepted() const;
                void setAccepted(bool);
                void accept();

            private:
                Type _eventType;
                bool _accepted = false;
            };

            //! This class provides an event for when an object's parent changes.
            class ParentChanged : public Event
            {
            public:
                ParentChanged(const std::shared_ptr<IObject> & prevParent, const std::shared_ptr<IObject> & newParent);

                const std::shared_ptr<IObject> & getPrevParent() const;
                const std::shared_ptr<IObject> & getNewParent() const;

            private:
                std::shared_ptr<IObject> _prevParent;
                std::shared_ptr<IObject> _newParent;
            };

            //! This class provides an event for when a child object is added.
            class ChildAdded : public Event
            {
            public:
                explicit ChildAdded(const std::shared_ptr<IObject> & child);

                const std::shared_ptr<IObject> & getChild() const;

            private:
                std::shared_ptr<IObject> _child;
            };

            //! This class provides an event for when a child object is removed.
            class ChildRemoved : public Event
            {
            public:
                explicit ChildRemoved(const std::shared_ptr<IObject> & child);

                const std::shared_ptr<IObject> & getChild() const;

            private:
                std::shared_ptr<IObject> _child;
            };

            //! This class provides an event for when the children change order.
            class ChildOrder : public Event
            {
            public:
                ChildOrder();
            };

            //! This class provides an initialization event.
            class Init : public Event
            {
            public:
                Init();
            };

            //! This class provides an update event.
            class Update : public Event
            {
            public:
                Update(const std::chrono::steady_clock::time_point&, const Time::Duration&);

                const std::chrono::steady_clock::time_point& getTime() const;
                const Time::Duration& getDeltaTime() const;

            private:
                std::chrono::steady_clock::time_point _t;
                Time::Duration _dt;
            };

            //! This class provides an event to initialize user interface layout.
            class InitLayout : public Event
            {
            public:
                InitLayout();
            };

            //! This class provides an event to prepare for user interface layout.
            class PreLayout : public Event
            {
            public:
                PreLayout();
            };

            //! This class provides an event for user interface layout.
            class Layout : public Event
            {
            public:
                Layout();
            };

            //! This class provides a clip event.
            class Clip : public Event
            {
            public:
                explicit Clip(const BBox2f & clipRect);

                const BBox2f & getClipRect() const;
                void setClipRect(const BBox2f &);

            private:
                BBox2f _clipRect;
            };

            //! This class provides a paint event.
            class Paint : public Event
            {
            public:
                explicit Paint(const BBox2f & clipRect);

                const BBox2f & getClipRect() const;
                void setClipRect(const BBox2f &);

            private:
                BBox2f _clipRect;
            };

            //! This class provides a second paint event after the children have been
            //! drawn.
            class PaintOverlay : public Event
            {
            public:
                explicit PaintOverlay(const BBox2f& clipRect);

                const BBox2f& getClipRect() const;
                void setClipRect(const BBox2f&);

            private:
                BBox2f _clipRect;
            };

            //! This typedef provides a pointer ID.
            typedef uint32_t PointerID;

            //! This constant provides an invalid pointer ID.
            const PointerID invalidID = 0;

            //! This struct provides information about the pointer.
            class PointerInfo
            {
            public:
                PointerInfo();
                
                PointerID id = invalidID;
                glm::vec3 pos = glm::vec3(0.F, 0.F, 0.F);
                glm::vec3 dir = glm::vec3(0.F, 0.F, 0.F);
                glm::vec2 projectedPos = glm::vec2(-1.F, -1.F);
                std::map<int, bool> buttons;

                bool operator == (const PointerInfo&) const;
            };

            //! This class provides the interface for pointer events.
            class IPointer : public Event
            {
            protected:
                IPointer(const PointerInfo &, Type);

            public:
                ~IPointer() override = 0;

                bool isRejected() const;
                void setRejected(bool);
                void reject();

                const PointerInfo & getPointerInfo() const;

            private:
                bool _rejected = false;
                PointerInfo _pointerInfo;
            };

            //! This class provides a pointer enter event.
            class PointerEnter : public IPointer
            {
            public:
                explicit PointerEnter(const PointerInfo &);
            };

            //! This class provides a pointer leave event.
            class PointerLeave : public IPointer
            {
            public:
                explicit PointerLeave(const PointerInfo &);
            };

            //! This class provides a pointer move event.
            class PointerMove : public IPointer
            {
            public:
                explicit PointerMove(const PointerInfo &);
            };

            //! This class provides a button press event.
            class ButtonPress : public IPointer
            {
            public:
                explicit ButtonPress(const PointerInfo &);
            };

            //! This class provides a button release event.
            class ButtonRelease : public IPointer
            {
            public:
                explicit ButtonRelease(const PointerInfo &);
            };

            //! This class provides a scroll event.
            class Scroll : public IPointer
            {
            public:
                Scroll(const glm::vec2 & scrollDelta, const PointerInfo &);

                const glm::vec2 & getScrollDelta() const;

            private:
                glm::vec2 _scrollDelta;
            };

            //! This class provides a drag and drop event.
            class Drop : public IPointer
            {
            public:
                Drop(const std::vector<std::string> &, const PointerInfo &);

                const std::vector<std::string> & getDropPaths() const;

            private:
                std::vector<std::string> _dropPaths;
            };

            //! This class provides the interface for key events.
            class IKey : public IPointer
            {
            protected:
                IKey(int key, int keyModifiers, const PointerInfo &, Type);
                
            public:
                ~IKey() override = 0;
                
                int getKey() const;
                int getKeyModifiers() const;

            private:
                int _key;
                int _keyModifiers;
            };

            //! This class provides a key press event.
            class KeyPress : public IKey
            {
            public:
                KeyPress(int key, int keyModifiers, const PointerInfo &);
            };

            //! This class provides a key release event.
            class KeyRelease : public IKey
            {
            public:
                KeyRelease(int key, int keyModifiers, const PointerInfo &);
            };

            //! This class provides a text focus event.
            class TextFocus : public Event
            {
            public:
                TextFocus();
            };

            //! This class provides a text focus lost event.
            class TextFocusLost : public Event
            {
            public:
                TextFocusLost();
            };

            //! This class provides a text input event.
            class TextInput : public Event
            {
            public:
                TextInput(const std::basic_string<djv_char_t>& utf32, int textModifiers);

                const std::basic_string<djv_char_t>& getUtf32() const;
                int getTextModifiers() const;

            private:
                std::basic_string<djv_char_t> _utf32;
                int _textModifiers;
            };

        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::Event::Type);

} // namespace djv

#include <djvCore/EventInline.h>
