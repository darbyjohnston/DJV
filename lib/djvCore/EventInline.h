// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace Event
        {
            inline Event::Event(Type type) :
                _eventType(type)
            {}

            inline Event::~Event()
            {}

            inline Type Event::getEventType() const
            {
                return _eventType;
            }

            inline void Event::setAccepted(bool value)
            {
                _accepted = value;
            }

            inline void Event::accept()
            {
                _accepted = true;
            }

            inline bool Event::isAccepted() const
            {
                return _accepted;
            }

            inline ParentChanged::ParentChanged(const std::shared_ptr<IObject>& prevParent, const std::shared_ptr<IObject>& newParent) :
                Event(Type::ParentChanged),
                _prevParent(prevParent),
                _newParent(newParent)
            {}

            inline const std::shared_ptr<IObject>& ParentChanged::getPrevParent() const
            {
                return _prevParent;
            }

            inline const std::shared_ptr<IObject>& ParentChanged::getNewParent() const
            {
                return _newParent;
            }

            inline ChildAdded::ChildAdded(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildAdded),
                _child(child)
            {}

            inline const std::shared_ptr<IObject>& ChildAdded::getChild() const
            {
                return _child;
            }

            inline ChildRemoved::ChildRemoved(const std::shared_ptr<IObject>& child) :
                Event(Type::ChildRemoved),
                _child(child)
            {}

            inline const std::shared_ptr<IObject>& ChildRemoved::getChild() const
            {
                return _child;
            }

            inline ChildOrder::ChildOrder() :
                Event(Type::ChildOrder)
            {}

            inline InitData::InitData(bool all) :
                redraw(all),
                resize(all),
                font(all),
                text(all)
            {}

            inline Init::Init(const InitData& data) :
                Event(Type::Init),
                _data(data)
            {}

            inline const InitData& Init::getData() const
            {
                return _data;
            }

            inline Update::Update(const std::chrono::steady_clock::time_point& t, const Time::Duration& dt) :
                Event(Type::Update),
                _t(t),
                _dt(dt)
            {}

            inline const std::chrono::steady_clock::time_point& Update::getTime() const
            {
                return _t;
            }

            inline const Time::Duration& Update::getDeltaTime() const
            {
                return _dt;
            }

            inline InitLayout::InitLayout() :
                Event(Type::InitLayout)
            {}

            inline PreLayout::PreLayout() :
                Event(Type::PreLayout)
            {}

            inline Layout::Layout() :
                Event(Type::Layout)
            {}

            inline Clip::Clip(const BBox2f& clipRect) :
                Event(Type::Clip),
                _clipRect(clipRect)
            {}

            inline const BBox2f& Clip::getClipRect() const
            {
                return _clipRect;
            }

            inline void Clip::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }

            inline Paint::Paint(const BBox2f& clipRect) :
                Event(Type::Paint),
                _clipRect(clipRect)
            {}

            inline const BBox2f& Paint::getClipRect() const
            {
                return _clipRect;
            }

            inline void Paint::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }

            inline PaintOverlay::PaintOverlay(const BBox2f& clipRect) :
                Event(Type::PaintOverlay),
                _clipRect(clipRect)
            {}

            inline const BBox2f& PaintOverlay::getClipRect() const
            {
                return _clipRect;
            }

            inline void PaintOverlay::setClipRect(const BBox2f& value)
            {
                _clipRect = value;
            }
            
            inline bool PointerInfo::operator == (const PointerInfo& other) const
            {
                return
                    id           == other.id           &&
                    pos          == other.pos          &&
                    dir          == other.dir          &&
                    projectedPos == other.projectedPos &&
                    buttons      == other.buttons;
            }

            inline IPointer::IPointer(const PointerInfo& pointerInfo, Type type) :
                Event(type),
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
 
            inline const PointerInfo& IPointer::getPointerInfo() const
            {
                return _pointerInfo;
            }

            inline PointerEnter::PointerEnter(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerEnter)
            {}

            inline PointerLeave::PointerLeave(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerLeave)
            {}

            inline PointerMove::PointerMove(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::PointerMove)
            {}

            inline ButtonPress::ButtonPress(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::ButtonPress)
            {}

            inline ButtonRelease::ButtonRelease(const PointerInfo& pointerInfo) :
                IPointer(pointerInfo, Type::ButtonRelease)
            {}

            inline Scroll::Scroll(const glm::vec2& scrollDelta, const PointerInfo& info) :
                IPointer(info, Type::Scroll),
                _scrollDelta(scrollDelta)
            {}

            inline const glm::vec2& Scroll::getScrollDelta() const
            {
                return _scrollDelta;
            }

            inline Drop::Drop(const std::vector<std::string>& dropPaths, const PointerInfo& info) :
                IPointer(info, Type::Drop),
                _dropPaths(dropPaths)
            {}

            inline const std::vector<std::string>& Drop::getDropPaths() const
            {
                return _dropPaths;
            }

            inline IKey::IKey(int key, int keyModifiers, const PointerInfo& info, Type type) :
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

            inline KeyPress::KeyPress(int key, int keyModifiers, const PointerInfo& info)  :
                IKey(key, keyModifiers, info, Type::KeyPress)
            {}

            inline KeyRelease::KeyRelease(int key, int keyModifiers, const PointerInfo& info) :
                IKey(key, keyModifiers, info, Type::KeyRelease)
            {}

            inline TextFocus::TextFocus() :
                Event(Type::TextFocus)
            {}

            inline TextFocusLost::TextFocusLost() :
                Event(Type::TextFocusLost)
            {}

            inline TextInput::TextInput(const std::basic_string<djv_char_t>& utf32, int textModifiers) :
                Event(Type::TextInput),
                _utf32(utf32),
                _textModifiers(textModifiers)
            {}

            inline const std::basic_string<djv_char_t>& TextInput::getUtf32() const
            {
                return _utf32;
            }

            inline int TextInput::getTextModifiers() const
            {
                return _textModifiers;
            }

        } // namespace Event
    } // namespace Core
} // namespace djv
