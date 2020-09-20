// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/Event.h>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace Event
        {
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
            
            IPointer::IPointer(const PointerInfo& pointerInfo, Type type) :
                Event(type),
                _pointerInfo(pointerInfo)
            {}

            IPointer::~IPointer()
            {}

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

            Drop::Drop(const std::vector<std::string>& dropPaths, const PointerInfo& info) :
                IPointer(info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            IKey::IKey(int key, int keyModifiers, const PointerInfo& info, Type type) :
                IPointer(info, type),
                _key(key),
                _keyModifiers(keyModifiers)
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
} // namespace djv

