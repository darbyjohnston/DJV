// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Event.h>

#include <algorithm>
#include <array>

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            Event::Event(Type type) :
                _eventType(type)
            {}

            Event::~Event()
            {}

            Type Event::getEventType() const
            {
                return _eventType;
            }

            void Event::setAccepted(bool value)
            {
                _accepted = value;
            }

            void Event::accept()
            {
                _accepted = true;
            }

            bool Event::isAccepted() const
            {
                return _accepted;
            }

            ParentChanged::ParentChanged(const std::shared_ptr<IObject>& prevParent, const std::shared_ptr<IObject>& newParent) :
                Event(Type::ParentChanged),
                _prevParent(prevParent),
                _newParent(newParent)
            {}

            const std::shared_ptr<IObject>& ParentChanged::getPrevParent() const
            {
                return _prevParent;
            }

            const std::shared_ptr<IObject>& ParentChanged::getNewParent() const
            {
                return _newParent;
            }

            ChildAdded::ChildAdded(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildAdded),
                _child(child)
            {}

            const std::shared_ptr<IObject>& ChildAdded::getChild() const
            {
                return _child;
            }

            ChildRemoved::ChildRemoved(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildRemoved),
                _child(child)
            {}

            const std::shared_ptr<IObject>& ChildRemoved::getChild() const
            {
                return _child;
            }

            ChildOrder::ChildOrder() :
                Event(Type::ChildOrder)
            {}

            InitData::InitData(bool all) :
                redraw(all),
                resize(all),
                font(all),
                text(all)
            {}

            Init::Init(const InitData& data) :
                Event(Type::Init),
                _data(data)
            {}

            const InitData& Init::getData() const
            {
                return _data;
            }

            Update::Update(const std::chrono::steady_clock::time_point& t, const Time::Duration& dt) :
                Event(Type::Update),
                _t(t),
                _dt(dt)
            {}

            const std::chrono::steady_clock::time_point& Update::getTime() const
            {
                return _t;
            }

            const Time::Duration& Update::getDeltaTime() const
            {
                return _dt;
            }

            InitLayout::InitLayout() :
                Event(Type::InitLayout)
            {}

            PreLayout::PreLayout() :
                Event(Type::PreLayout)
            {}

            Layout::Layout() :
                Event(Type::Layout)
            {}

            Clip::Clip(const BBox2f& clipRect) :
                Event(Type::Clip),
                _clipRect(clipRect)
            {}

            const BBox2f& Clip::getClipRect() const
            {
                return _clipRect;
            }

            void Clip::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }

            Paint::Paint(const BBox2f& clipRect) :
                Event(Type::Paint),
                _clipRect(clipRect)
            {}

            const BBox2f& Paint::getClipRect() const
            {
                return _clipRect;
            }

            void Paint::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }

            PaintOverlay::PaintOverlay(const BBox2f& clipRect) :
                Event(Type::PaintOverlay),
                _clipRect(clipRect)
            {}

            const BBox2f& PaintOverlay::getClipRect() const
            {
                return _clipRect;
            }

            void PaintOverlay::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }
            
            PointerInfo::PointerInfo()
            {}
            
            bool PointerInfo::operator == (const PointerInfo& other) const
            {
                return
                    id           == other.id           &&
                    pos          == other.pos          &&
                    dir          == other.dir          &&
                    projectedPos == other.projectedPos &&
                    buttons      == other.buttons;
            }

            IPointer::IPointer(const PointerInfo& pointerInfo, Type type) :
                Event(type),
                _pointerInfo(pointerInfo)
            {}

            IPointer::~IPointer()
            {}

            bool IPointer::isRejected() const
            {
                return _rejected;
            }

            void IPointer::setRejected(bool value)
            {
                _rejected = value;
            }

            void IPointer::reject()
            {
                _rejected = true;
            }
 
            const PointerInfo& IPointer::getPointerInfo() const
            {
                return _pointerInfo;
            }

            PointerEnter::PointerEnter(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerEnter)
            {}

            PointerLeave::PointerLeave(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerLeave)
            {}

            PointerMove::PointerMove(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerMove)
            {}

            ButtonPress::ButtonPress(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::ButtonPress)
            {}

            ButtonRelease::ButtonRelease(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::ButtonRelease)
            {}

            Scroll::Scroll(const glm::vec2& scrollDelta, const PointerInfo& info) :
                IPointer(info, Type::Scroll),
                _scrollDelta(scrollDelta)
            {}

            const glm::vec2& Scroll::getScrollDelta() const
            {
                return _scrollDelta;
            }

            Drop::Drop(const std::vector<std::string>& dropPaths, const PointerInfo& info) :
                IPointer(info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            const std::vector<std::string>& Drop::getDropPaths() const
            {
                return _dropPaths;
            }

            IKey::IKey(int key, int keyModifiers, const PointerInfo& info, Type type) :
                IPointer(info, type),
                _key(key),
                _keyModifiers(keyModifiers)
            {}

            IKey::~IKey()
            {}

            int IKey::getKey() const
            {
                return _key;
            }

            int IKey::getKeyModifiers() const
            {
                return _keyModifiers;
            }

            KeyPress::KeyPress(int key, int keyModifiers, const PointerInfo& info)  :
                IKey(key, keyModifiers, info, Type::KeyPress)
            {}

            KeyRelease::KeyRelease(int key, int keyModifiers, const PointerInfo& info) :
                IKey(key, keyModifiers, info, Type::KeyRelease)
            {}

            TextFocus::TextFocus() :
                Event(Type::TextFocus)
            {}

            TextFocusLost::TextFocusLost() :
                Event(Type::TextFocusLost)
            {}

            TextInput::TextInput(const std::basic_string<djv_char_t>& utf32, int textModifiers) :
                Event(Type::TextInput),
                _utf32(utf32),
                _textModifiers(textModifiers)
            {}

            const std::basic_string<djv_char_t>& TextInput::getUtf32() const
            {
                return _utf32;
            }

            int TextInput::getTextModifiers() const
            {
                return _textModifiers;
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(Type);

    } // namespace Event
    } // namespace Core
        
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Event,
        Type,
        DJV_TEXT("event_parent_changed"),
        DJV_TEXT("event_child_added"),
        DJV_TEXT("event_child_removed"),
        DJV_TEXT("event_child_order"),
        DJV_TEXT("event_init"),
        DJV_TEXT("event_update"),
        DJV_TEXT("event_init_layout"),
        DJV_TEXT("event_pre_layout"),
        DJV_TEXT("event_layout"),
        DJV_TEXT("event_clip"),
        DJV_TEXT("event_paint"),
        DJV_TEXT("event_paint_overlay"),
        DJV_TEXT("event_pointer_enter"),
        DJV_TEXT("event_pointer_leave"),
        DJV_TEXT("event_pointer_move"),
        DJV_TEXT("event_button_press"),
        DJV_TEXT("event_button_release"),
        DJV_TEXT("event_scroll"),
        DJV_TEXT("event_drop"),
        DJV_TEXT("event_key_press"),
        DJV_TEXT("event_key_release"),
        DJV_TEXT("event_text_focus"),
        DJV_TEXT("event_text_focus_lost"),
        DJV_TEXT("event_text_input"));

} // namespace djv
