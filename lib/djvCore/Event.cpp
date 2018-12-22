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
        namespace Event
        {
            IEvent::IEvent(Type type) :
                _eventType(type)
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

            Update::Update(float dt) :
                IEvent(Type::Update),
                _dt(dt)
            {}

            Locale::Locale(const std::string& locale) :
                IEvent(Type::Locale),
                _locale(locale)
            {}

            PreLayout::PreLayout() :
                IEvent(Type::PreLayout)
            {}

            Layout::Layout() :
                IEvent(Type::Layout)
            {}

            Clip::Clip(const BBox2f& clipRect) :
                IEvent(Type::Clip),
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
                IEvent(Type::Paint),
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

            IPointer::IPointer(const PointerInfo& pointerInfo, Type type) :
                IEvent(type),
                _pointerInfo(pointerInfo)
            {}

            IPointer::~IPointer()
            {}

            void IPointer::setRejected(bool value)
            {
                _rejected = value;
            }

            void IPointer::reject()
            {
                _rejected = true;
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

            Drop::Drop(const std::vector<std::string>& dropPaths, const PointerInfo& info) :
                IPointer(info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            KeyboardFocus::KeyboardFocus() :
                IEvent(Type::KeyboardFocus)
            {}

            KeyboardFocusLost::KeyboardFocusLost() :
                IEvent(Type::KeyboardFocusLost)
            {}

            IKey::IKey(int key, int keyModifiers, const PointerInfo& info, Type type) :
                IPointer(info, type),
                _key(key),
                _keyModifiers(keyModifiers)
            {}

            IKey::~IKey()
            {}

            KeyPress::KeyPress(int key, int keyModifiers, const PointerInfo& info) :
                IKey(key, keyModifiers, info, Type::KeyPress)
            {}

            KeyRelease::KeyRelease(int key, int keyModifiers, const PointerInfo& info) :
                IKey(key, keyModifiers, info, Type::KeyRelease)
            {}

            Text::Text(const std::string& text) :
                IEvent(Type::Text),
                _text(text)
            {}

        } // namespace Event
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Event,
        Type,
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
