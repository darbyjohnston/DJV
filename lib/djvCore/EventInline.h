//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            inline IEvent::IEvent(Type type) :
                _eventType(type)
            {}

            inline IEvent::~IEvent()
            {}

            inline Type IEvent::getEventType() const
            {
                return _eventType;
            }

            inline void IEvent::setAccepted(bool value)
            {
                _accepted = value;
            }

            inline void IEvent::accept()
            {
                _accepted = true;
            }

            inline bool IEvent::isAccepted() const
            {
                return _accepted;
            }

            inline ParentChanged::ParentChanged(const std::shared_ptr<IObject> & prevParent, const std::shared_ptr<IObject> & newParent) :
                IEvent(Type::ParentChanged),
                _prevParent(prevParent),
                _newParent(newParent)
            {}

            inline const std::shared_ptr<IObject> & ParentChanged::getPrevParent() const
            {
                return _prevParent;
            }

            inline const std::shared_ptr<IObject> & ParentChanged::getNewParent() const
            {
                return _newParent;
            }

            inline ChildAdded::ChildAdded(const std::shared_ptr<IObject> & child) :
                IEvent(Type::ChildAdded),
                _child(child)
            {}

            inline const std::shared_ptr<IObject> & ChildAdded::getChild() const
            {
                return _child;
            }

            inline ChildRemoved::ChildRemoved(const std::shared_ptr<IObject> & child) :
                IEvent(Type::ChildRemoved),
                _child(child)
            {}

            inline const std::shared_ptr<IObject> & ChildRemoved::getChild() const
            {
                return _child;
            }

            inline ChildOrder::ChildOrder() :
                IEvent(Type::ChildOrder)
            {}

            inline Locale::Locale(const std::string & locale) :
                IEvent(Type::Locale),
                _locale(locale)
            {}

            inline const std::string & Locale::getLocale() const
            {
                return _locale;
            }

            inline Update::Update(float t, float dt) :
                IEvent(Type::Update),
                _t(t),
                _dt(dt)
            {}

            inline float Update::getTime() const
            {
                return _t;
            }

            inline float Update::getDeltaTime() const
            {
                return _dt;
            }

            inline Style::Style() :
                IEvent(Type::Style)
            {}

            inline PreLayout::PreLayout() :
                IEvent(Type::PreLayout)
            {}

            inline Layout::Layout() :
                IEvent(Type::Layout)
            {}

            inline Clip::Clip(const BBox2f & clipRect) :
                IEvent(Type::Clip),
                _clipRect(clipRect)
            {}

            inline const BBox2f & Clip::getClipRect() const
            {
                return _clipRect;
            }

            inline void Clip::setClipRect(const BBox2f & value)
            {
                _clipRect = value;
            }

            inline Paint::Paint(const BBox2f & clipRect) :
                IEvent(Type::Paint),
                _clipRect(clipRect)
            {}

            inline const BBox2f & Paint::getClipRect() const
            {
                return _clipRect;
            }

            inline void Paint::setClipRect(const BBox2f & value)
            {
                _clipRect = value;
            }

            inline PaintOverlay::PaintOverlay(const BBox2f & clipRect) :
                IEvent(Type::PaintOverlay),
                _clipRect(clipRect)
            {}

            inline const BBox2f & PaintOverlay::getClipRect() const
            {
                return _clipRect;
            }

            inline void PaintOverlay::setClipRect(const BBox2f & value)
            {
                _clipRect = value;
            }

            inline IPointer::IPointer(const PointerInfo & pointerInfo, Type type) :
                IEvent(type),
                _pointerInfo(pointerInfo)
            {}

            inline IPointer::~IPointer()
            {}

            inline bool IPointer::isRejected() const
            {
                return _rejected;
            }

            inline void IPointer::setRejected(bool value)
            {
                _rejected = value;
            }

            inline void IPointer::reject()
            {
                _rejected = true;
            }
 
            inline const PointerInfo & IPointer::getPointerInfo() const
            {
                return _pointerInfo;
            }

            inline PointerEnter::PointerEnter(const PointerInfo & pointerInfo) :
                IPointer(pointerInfo, Type::PointerEnter)
            {}

            inline PointerLeave::PointerLeave(const PointerInfo & pointerInfo) :
                IPointer(pointerInfo, Type::PointerLeave)
            {}

            inline PointerMove::PointerMove(const PointerInfo & pointerInfo) :
                IPointer(pointerInfo, Type::PointerMove)
            {}

            inline ButtonPress::ButtonPress(const PointerInfo & pointerInfo) :
                IPointer(pointerInfo, Type::ButtonPress)
            {}

            inline ButtonRelease::ButtonRelease(const PointerInfo & pointerInfo) :
                IPointer(pointerInfo, Type::ButtonRelease)
            {}

            inline Scroll::Scroll(const glm::vec2 & scrollDelta, const PointerInfo & info) :
                IPointer(info, Type::Scroll),
                _scrollDelta(scrollDelta)
            {}

            inline const glm::vec2 & Scroll::getScrollDelta() const
            {
                return _scrollDelta;
            }

            inline Drop::Drop(const std::vector<std::string>& dropPaths, const PointerInfo & info) :
                IPointer(info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            inline const std::vector<std::string>& Drop::getDropPaths() const
            {
                return _dropPaths;
            }

            inline IKey::IKey(int key, int keyModifiers, const PointerInfo & info, Type type) :
                IPointer(info, type),
                _key(key),
                _keyModifiers(keyModifiers)
            {}

            inline IKey::~IKey()
            {}

            inline int IKey::getKey() const
            {
                return _key;
            }

            inline int IKey::getKeyModifiers() const
            {
                return _keyModifiers;
            }

            inline KeyPress::KeyPress(int key, int keyModifiers, const PointerInfo & info)  :
                IKey(key, keyModifiers, info, Type::KeyPress)
            {}

            inline KeyRelease::KeyRelease(int key, int keyModifiers, const PointerInfo & info) :
                IKey(key, keyModifiers, info, Type::KeyRelease)
            {}

            inline TextFocus::TextFocus() :
                IEvent(Type::TextFocus)
            {}

            inline TextFocusLost::TextFocusLost() :
                IEvent(Type::TextFocusLost)
            {}

            inline Text::Text(const std::string & text, int textModifiers) :
                IEvent(Type::Text),
                _text(text),
                _textModifiers(textModifiers)
            {}

            inline const std::string & Text::getText() const
            {
                return _text;
            }

            inline int Text::getTextModifiers() const
            {
                return _textModifiers;
            }

        } // namespace Event
    } // namespace Core
} // namespace djv
