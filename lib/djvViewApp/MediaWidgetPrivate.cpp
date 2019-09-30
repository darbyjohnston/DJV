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

#include <djvViewApp/MediaWidgetPrivate.h>

#include <djvUI/Action.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        void PointerWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::ViewApp::MediaWidget::PointerWidget");
        }

        std::shared_ptr<PointerWidget> PointerWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PointerWidget>(new PointerWidget);
            out->_init(context);
            return out;
        }

        void PointerWidget::setHoverCallback(const std::function<void(PointerData)>& callback)
        {
            _hoverCallback = callback;
        }

        void PointerWidget::setDragCallback(const std::function<void(PointerData)>& callback)
        {
            _dragCallback = callback;
        }

        void PointerWidget::_pointerEnterEvent(Event::PointerEnter& event)
        {
            if (!event.isRejected())
            {
                event.accept();
                const auto& pos = event.getPointerInfo().projectedPos;
                _doHoverCallback(PointerData(PointerState::Start, pos, std::map<int, bool>()));
            }
        }

        void PointerWidget::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            _doHoverCallback(PointerData(PointerState::End, pos, std::map<int, bool>()));
        }

        void PointerWidget::_pointerMoveEvent(Event::PointerMove& event)
        {
            event.accept();
            const auto& pos = event.getPointerInfo().projectedPos;
            if (_pressedID)
            {
                _doDragCallback(PointerData(PointerState::Move, pos, _buttons));
            }
            else
            {
                _doHoverCallback(PointerData(PointerState::Move, pos, std::map<int, bool>()));
            }
        }

        void PointerWidget::_buttonPressEvent(Event::ButtonPress& event)
        {
            if (_pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            _pressedID = info.id;
            _buttons = info.buttons;
            _doDragCallback(PointerData(PointerState::Start, info.pos, info.buttons));
        }

        void PointerWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            if (event.getPointerInfo().id != _pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            _pressedID = Event::InvalidID;
            _buttons = std::map<int, bool>();
            _doDragCallback(PointerData(PointerState::End, info.pos, info.buttons));
        }

        void PointerWidget::_doHoverCallback(const PointerData& value)
        {
            if (_hoverCallback)
            {
                _hoverCallback(value);
            }
        }

        void PointerWidget::_doDragCallback(const PointerData& value)
        {
            if (_dragCallback)
            {
                _dragCallback(value);
            }
        }

        void FrameWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            setClassName("djv::ViewApp::MediaWidget::FrameWidget");

            auto nextAction = UI::Action::create();
            nextAction->setShortcut(GLFW_KEY_UP);
            auto prevAction = UI::Action::create();
            prevAction->setShortcut(GLFW_KEY_DOWN);
            auto nextX10Action = UI::Action::create();
            nextX10Action->setShortcut(GLFW_KEY_PAGE_UP);
            auto prevX10Action = UI::Action::create();
            prevX10Action->setShortcut(GLFW_KEY_PAGE_DOWN);

            _lineEdit = UI::LineEdit::create(context);
            _lineEdit->setFont(AV::Font::familyMono);
            _lineEdit->setFontSizeRole(UI::MetricsRole::FontSmall);
            _lineEdit->setBackgroundRole(UI::ColorRole::None);
            _lineEdit->addAction(nextAction);
            _lineEdit->addAction(prevAction);
            _lineEdit->addAction(nextX10Action);
            _lineEdit->addAction(prevX10Action);
            addChild(_lineEdit);
            
            _widgetUpdate();
            
            auto weak = std::weak_ptr<FrameWidget>(std::dynamic_pointer_cast<FrameWidget>(shared_from_this()));
            _actionObservers["Next"] = ValueObserver<bool>::create(
                nextAction->observeClicked(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index + 1);
                    }
                });

            _actionObservers["Prev"] = ValueObserver<bool>::create(
                prevAction->observeClicked(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index - 1);
                    }
                });
            _actionObservers["NextX10"] = ValueObserver<bool>::create(
                nextX10Action->observeClicked(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index + 10);
                    }
                });
            _actionObservers["PrevX10"] = ValueObserver<bool>::create(
                prevX10Action->observeClicked(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index - 10);
                    }
                });

            _lineEdit->setTextFinishedCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        Frame::Index index = Frame::invalidIndex;
                        switch (widget->_timeUnits)
                        {
                        case AV::TimeUnits::Timecode:
                        {
                            uint32_t timecode = 0;
                            bool ok = false;
                            Time::stringToTimecode(value, timecode, &ok);
                            if (ok)
                            {
                                const Frame::Number frame = Time::timecodeToFrame(timecode, widget->_speed);
                                index = widget->_sequence.getIndex(frame);
                            }
                            break;
                        }
                        case AV::TimeUnits::Frames:
                        {
                            try
                            {
                                const Frame::Number frame = std::stoi(value);
                                index = widget->_sequence.getIndex(frame);
                            }
                            catch (const std::exception& e)
                            {
                                widget->_log(e.what(), LogLevel::Error);
                            }
                            break;
                        }
                        default: break;
                        }
                        widget->_setFrame(index);
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            _timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::TimeUnits value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_timeUnits = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        std::shared_ptr<FrameWidget> FrameWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FrameWidget>(new FrameWidget);
            out->_init(context);
            return out;
        }

        void FrameWidget::setSequence(const Frame::Sequence& value)
        {
            if (value == _sequence)
                return;
            _sequence = value;
            _widgetUpdate();
        }

        void FrameWidget::setSpeed(const Time::Speed& value)
        {
            if (value == _speed)
                return;
            _speed = value;
            _widgetUpdate();
        }

        void FrameWidget::setFrame(const Frame::Index value)
        {
            if (value == _index)
                return;
            _index = value;
            _widgetUpdate();
        }

        void FrameWidget::setCallback(const std::function<void(Frame::Index)>& value)
        {
            _callback = value;
        }

        void FrameWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_lineEdit->getMinimumSize());
        }

        void FrameWidget::_layoutEvent(Event::Layout&)
        {
            _lineEdit->setGeometry(getGeometry());
        }

        void FrameWidget::_setFrame(Frame::Index value)
        {
            if (value != Frame::invalidIndex)
            {
                _index = value;
                if (_callback)
                {
                    _callback(_index);
                }
            }
            _widgetUpdate();
        }

        void FrameWidget::_widgetUpdate()
        {
            switch (_timeUnits)
            {
            case AV::TimeUnits::Timecode:
            {
                _lineEdit->setSizeString("00:00:00:00");
                const uint32_t timecode = Time::frameToTimecode(_sequence.getFrame(_index), _speed);
                const std::string s = Time::timecodeToString(timecode);
                _lineEdit->setText(s);
                break;
            }
            case AV::TimeUnits::Frames:
            {
                _lineEdit->setSizeString("00000");
                const Frame::Number frame = _sequence.getFrame(_index);
                std::stringstream ss;
                ss << frame;
                _lineEdit->setText(ss.str());
                break;
            }
            default: break;
            }
        }

    } // namespace ViewApp
} // namespace djv

