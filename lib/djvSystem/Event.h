// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/BBox.h>

#include <djvCore/Enum.h>
#include <djvCore/Time.h>

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace System
    {
        class IObject;

        //! Event system.
        namespace Event
        {
            //! Event types.
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

            //! Base class for events.
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

            //! Parent changed event.
            class ParentChanged : public Event
            {
            public:
                ParentChanged(const std::shared_ptr<IObject>& prevParent, const std::shared_ptr<IObject>& newParent);

                const std::shared_ptr<IObject>& getPrevParent() const;
                const std::shared_ptr<IObject>& getNewParent() const;

            private:
                std::shared_ptr<IObject> _prevParent;
                std::shared_ptr<IObject> _newParent;
            };

            //! Child added event.
            class ChildAdded : public Event
            {
            public:
                explicit ChildAdded(const std::shared_ptr<IObject>& child);

                const std::shared_ptr<IObject>& getChild() const;

            private:
                std::shared_ptr<IObject> _child;
            };

            //! Child removed event.
            class ChildRemoved : public Event
            {
            public:
                explicit ChildRemoved(const std::shared_ptr<IObject>& child);

                const std::shared_ptr<IObject>& getChild() const;

            private:
                std::shared_ptr<IObject> _child;
            };

            //! Child order changed event.
            class ChildOrder : public Event
            {
            public:
                ChildOrder();
            };

            //! Initialization event data.
            struct InitData
            {
                InitData(bool all = false);

                bool redraw = false;
                bool resize = false;
                bool font   = false;
                bool text   = false;
            };

            //! Initialization event.
            class Init : public Event
            {
            public:
                Init(const InitData& = InitData());

                const InitData& getData() const;

            private:
                InitData _data;
            };

            //! Update event.
            class Update : public Event
            {
            public:
                Update(const std::chrono::steady_clock::time_point&, const Core::Time::Duration&);

                const std::chrono::steady_clock::time_point& getTime() const;
                const Core::Time::Duration& getDeltaTime() const;

            private:
                std::chrono::steady_clock::time_point _t;
                Core::Time::Duration _dt;
            };

            //! Initialize user interface layout event.
            class InitLayout : public Event
            {
            public:
                InitLayout();
            };

            //! Prepare for user interface layout event.
            class PreLayout : public Event
            {
            public:
                PreLayout();
            };

            //! User interface layout event.
            class Layout : public Event
            {
            public:
                Layout();
            };

            //! Clip event.
            class Clip : public Event
            {
            public:
                explicit Clip(const Math::BBox2f& clipRect);

                const Math::BBox2f& getClipRect() const;

                void setClipRect(const Math::BBox2f&);

            private:
                Math::BBox2f _clipRect;
            };

            //! Paint event.
            class Paint : public Event
            {
            public:
                explicit Paint(const Math::BBox2f& clipRect);

                const Math::BBox2f& getClipRect() const;

                void setClipRect(const Math::BBox2f&);

            private:
                Math::BBox2f _clipRect;
            };

            //! Second paint event after the children have been drawn.
            class PaintOverlay : public Event
            {
            public:
                explicit PaintOverlay(const Math::BBox2f& clipRect);

                const Math::BBox2f& getClipRect() const;

                void setClipRect(const Math::BBox2f&);

            private:
                Math::BBox2f _clipRect;
            };

            //! Pointer ID.
            typedef uint32_t PointerID;

            //! Invalid pointer ID.
            const PointerID invalidID = 0;

            //! Pointer information.
            struct PointerInfo
            {
                PointerInfo();
                
                PointerID           id              = invalidID;
                glm::vec3           pos             = glm::vec3(0.F, 0.F, 0.F);
                glm::vec3           dir             = glm::vec3(0.F, 0.F, 0.F);
                glm::vec2           projectedPos    = glm::vec2(-1.F, -1.F);
                std::map<int, bool> buttons;

                bool operator == (const PointerInfo&) const;
            };

            //! Base class for pointer events.
            class IPointer : public Event
            {
            protected:
                IPointer(int keyModifiers, const PointerInfo&, Type);

            public:
                ~IPointer() override = 0;

                bool isRejected() const;

                void setRejected(bool);
                void reject();

                int getKeyModifiers() const;

                const PointerInfo& getPointerInfo() const;

            private:
                bool _rejected;
                int _keyModifiers;
                PointerInfo _pointerInfo;
            };

            //! Pointer enter event.
            class PointerEnter : public IPointer
            {
            public:
                explicit PointerEnter(int keyModifiers, const PointerInfo&);
            };

            //! Pointer leave event.
            class PointerLeave : public IPointer
            {
            public:
                explicit PointerLeave(int keyModifiers, const PointerInfo&);
            };

            //! Pointer move event.
            class PointerMove : public IPointer
            {
            public:
                explicit PointerMove(int keyModifiers, const PointerInfo&);
            };

            //! Button press event.
            class ButtonPress : public IPointer
            {
            public:
                explicit ButtonPress(int keyModifiers, const PointerInfo&);
            };

            //! Button release event.
            class ButtonRelease : public IPointer
            {
            public:
                explicit ButtonRelease(int keyModifiers, const PointerInfo&);
            };

            //! Scroll event.
            class Scroll : public IPointer
            {
            public:
                Scroll(const glm::vec2& scrollDelta, int keyModifiers, const PointerInfo&);

                const glm::vec2& getScrollDelta() const;

            private:
                glm::vec2 _scrollDelta;
            };

            //! Drag and drop event.
            class Drop : public IPointer
            {
            public:
                Drop(const std::vector<std::string>&, int keyModifiers, const PointerInfo&);

                const std::vector<std::string>& getDropPaths() const;

            private:
                std::vector<std::string> _dropPaths;
            };

            //! Base class for key events.
            class IKey : public IPointer
            {
            protected:
                IKey(int key, int keyModifiers, const PointerInfo&, Type);
                
            public:
                ~IKey() override = 0;
                
                int getKey() const;

            private:
                int _key;
            };

            //! Key press event.
            class KeyPress : public IKey
            {
            public:
                KeyPress(int key, int keyModifiers, const PointerInfo&);
            };

            //! Key release event.
            class KeyRelease : public IKey
            {
            public:
                KeyRelease(int key, int keyModifiers, const PointerInfo&);
            };

            //! Text focus event.
            class TextFocus : public Event
            {
            public:
                TextFocus();
            };

            //! Text focus lost event.
            class TextFocusLost : public Event
            {
            public:
                TextFocusLost();
            };

            //! Text input event.
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
    } // namespace System

    DJV_ENUM_SERIALIZE_HELPERS(System::Event::Type);

} // namespace djv

#include <djvSystem/EventInline.h>
