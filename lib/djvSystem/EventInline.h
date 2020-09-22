// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace System
    {
        namespace Event
        {
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

            inline const std::shared_ptr<IObject>& ParentChanged::getPrevParent() const
            {
                return _prevParent;
            }

            inline const std::shared_ptr<IObject>& ParentChanged::getNewParent() const
            {
                return _newParent;
            }

            inline const std::shared_ptr<IObject>& ChildAdded::getChild() const
            {
                return _child;
            }

            inline const std::shared_ptr<IObject>& ChildRemoved::getChild() const
            {
                return _child;
            }

            inline const InitData& Init::getData() const
            {
                return _data;
            }

            inline const std::chrono::steady_clock::time_point& Update::getTime() const
            {
                return _t;
            }

            inline const Core::Time::Duration& Update::getDeltaTime() const
            {
                return _dt;
            }

            inline const Math::BBox2f& Clip::getClipRect() const
            {
                return _clipRect;
            }

            inline void Clip::setClipRect(const Math::BBox2f& value)
            {
                _clipRect = value;
            }

            inline const Math::BBox2f& Paint::getClipRect() const
            {
                return _clipRect;
            }

            inline void Paint::setClipRect(const Math::BBox2f& value)
            {
                _clipRect = value;
            }

            inline const Math::BBox2f& PaintOverlay::getClipRect() const
            {
                return _clipRect;
            }

            inline void PaintOverlay::setClipRect(const Math::BBox2f& value)
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

            inline const glm::vec2& Scroll::getScrollDelta() const
            {
                return _scrollDelta;
            }

            inline const std::vector<std::string>& Drop::getDropPaths() const
            {
                return _dropPaths;
            }

            inline int IKey::getKey() const
            {
                return _key;
            }

            inline int IKey::getKeyModifiers() const
            {
                return _keyModifiers;
            }

            inline const std::basic_string<djv_char_t>& TextInput::getUtf32() const
            {
                return _utf32;
            }

            inline int TextInput::getTextModifiers() const
            {
                return _textModifiers;
            }

        } // namespace Event
    } // namespace System
} // namespace djv

