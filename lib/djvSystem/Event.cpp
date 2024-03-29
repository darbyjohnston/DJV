// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Event.h>

#include <algorithm>
#include <array>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace Event
        {
            DJV_ENUM_HELPERS_IMPLEMENTATION(Type);
            
            Event::Event(Type type) :
                _eventType(type)
            {}

            Event::~Event()
            {}

            ParentChanged::ParentChanged(const std::shared_ptr<IObject>& prevParent, const std::shared_ptr<IObject>& newParent) :
                Event(Type::ParentChanged),
                _prevParent(prevParent),
                _newParent(newParent)
            {}

            ChildAdded::ChildAdded(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildAdded),
                _child(child)
            {}

            ChildRemoved::ChildRemoved(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildRemoved),
                _child(child)
            {}

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

            Update::Update(const std::chrono::steady_clock::time_point& t, const Time::Duration& dt) :
                Event(Type::Update),
                _t(t),
                _dt(dt)
            {}

            InitLayout::InitLayout() :
                Event(Type::InitLayout)
            {}

            PreLayout::PreLayout() :
                Event(Type::PreLayout)
            {}

            Layout::Layout() :
                Event(Type::Layout)
            {}

            Clip::Clip(const Math::BBox2f& clipRect) :
                Event(Type::Clip),
                _clipRect(clipRect)
            {}

            Paint::Paint(const Math::BBox2f& clipRect) :
                Event(Type::Paint),
                _clipRect(clipRect)
            {}

            PaintOverlay::PaintOverlay(const Math::BBox2f& clipRect) :
                Event(Type::PaintOverlay),
                _clipRect(clipRect)
            {}
            
            PointerInfo::PointerInfo()
            {}
            
            IPointer::IPointer(int keyModifiers, const PointerInfo& pointerInfo, Type type) :
                Event(type),
                _rejected(false),
                _keyModifiers(keyModifiers),
                _pointerInfo(pointerInfo)
            {}

            IPointer::~IPointer()
            {}

            PointerEnter::PointerEnter(int keyModifiers, const PointerInfo& pointerInfo) :
                IPointer(keyModifiers, pointerInfo, Type::PointerEnter)
            {}

            PointerLeave::PointerLeave(int keyModifiers, const PointerInfo& pointerInfo) :
                IPointer(keyModifiers, pointerInfo, Type::PointerLeave)
            {}

            PointerMove::PointerMove(int keyModifiers, const PointerInfo& pointerInfo) :
                IPointer(keyModifiers, pointerInfo, Type::PointerMove)
            {}

            ButtonPress::ButtonPress(int keyModifiers, const PointerInfo& pointerInfo) :
                IPointer(keyModifiers, pointerInfo, Type::ButtonPress)
            {}

            ButtonRelease::ButtonRelease(int keyModifiers, const PointerInfo& pointerInfo) :
                IPointer(keyModifiers, pointerInfo, Type::ButtonRelease)
            {}

            Scroll::Scroll(const glm::vec2& scrollDelta, int keyModifiers, const PointerInfo& info) :
                IPointer(keyModifiers, info, Type::Scroll),
                _scrollDelta(scrollDelta)
            {}

            Drop::Drop(const std::vector<std::string>& dropPaths, int keyModifiers, const PointerInfo& info) :
                IPointer(keyModifiers, info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            IKey::IKey(int key, int keyModifiers, const PointerInfo& info, Type type) :
                IPointer(keyModifiers, info, type),
                _key(key)
            {}

            IKey::~IKey()
            {}

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

        } // namespace Event
    } // namespace System
        
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        System::Event,
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

