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
#include <djvUI/DrawUtil.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2D.h>

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
            setVAlign(UI::VAlign::Center);

            auto nextAction = UI::Action::create();
            nextAction->setShortcut(GLFW_KEY_UP);
            auto prevAction = UI::Action::create();
            prevAction->setShortcut(GLFW_KEY_DOWN);
            auto nextX10Action = UI::Action::create();
            nextX10Action->setShortcut(GLFW_KEY_PAGE_UP);
            auto prevX10Action = UI::Action::create();
            prevX10Action->setShortcut(GLFW_KEY_PAGE_DOWN);

            _lineEditBase = UI::LineEditBase::create(context);
            _lineEditBase->setFont(AV::Font::familyMono);
            _lineEditBase->setFontSizeRole(UI::MetricsRole::FontSmall);
            _lineEditBase->setBackgroundRole(UI::ColorRole::None);
            _lineEditBase->addAction(nextAction);
            _lineEditBase->addAction(prevAction);
            _lineEditBase->addAction(nextX10Action);
            _lineEditBase->addAction(prevX10Action);

            _buttons = UI::NumericEditButtons::create(context);
            _buttons->setBackgroundRole(UI::ColorRole::None);

            _layout = UI::HorizontalLayout::create(context);
            _layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            _layout->addChild(_lineEditBase);
            _layout->setStretch(_lineEditBase, UI::RowStretch::Expand);
            _layout->addChild(_buttons);
            addChild(_layout);
            
            _widgetUpdate();
            
            auto weak = std::weak_ptr<FrameWidget>(std::dynamic_pointer_cast<FrameWidget>(shared_from_this()));
            _lineEditBase->setTextEditCallback(
                [weak](const std::string& value, UI::TextEdit)
                {
                    if (auto widget = weak.lock())
                    {
                        Frame::Index index = Frame::invalidIndex;
                        switch (widget->_timeUnits)
                        {
                        case AV::TimeUnits::Timecode:
                        {
                            uint32_t timecode = 0;
                            try
                            {
                                Time::stringToTimecode(value, timecode);
                                const Frame::Number frame = Time::timecodeToFrame(timecode, widget->_speed);
                                index = widget->_sequence.getIndex(frame);
                            }
                            catch (const std::exception&)
                            {
                                std::stringstream ss;
                                ss << "Cannot parse the value.";
                                widget->_log(ss.str(), LogLevel::Error);
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
                            catch (const std::exception&)
                            {
                                std::stringstream ss;
                                ss << "Cannot parse the value.";
                                widget->_log(ss.str(), LogLevel::Error);
                            }
                            break;
                        }
                        default: break;
                        }
                        widget->_setFrame(index);
                    }
                });
            _lineEditBase->setFocusCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_redraw();
                    }
                });

            _buttons->setIncrementCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index + 1);
                    }
                });
            _buttons->setDecrementCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_index - 1);
                    }
                });

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
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            _setMinimumSize(_layout->getMinimumSize() + b * 6.F);
        }

        void FrameWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            _layout->setGeometry(getGeometry().margin(-b * 3.0F));
        }

        void FrameWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float b = style->getMetric(UI::MetricsRole::Border);
            auto render = _getRender();
            if (_lineEditBase->hasTextFocus())
            {
                render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                UI::drawBorder(render, g, b * 2.F);
            }
            render->setFillColor(style->getColor(UI::ColorRole::Border));
            const BBox2f g2 = g.margin(-b * 2.F);
            UI::drawBorder(render, g2, b);
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
                _lineEditBase->setSizeString("00:00:00:00");
                const uint32_t timecode = Time::frameToTimecode(_sequence.getFrame(_index), _speed);
                const std::string s = Time::timecodeToString(timecode);
                _lineEditBase->setText(s);
                break;
            }
            case AV::TimeUnits::Frames:
            {
                _lineEditBase->setSizeString("00000");
                const Frame::Number frame = _sequence.getFrame(_index);
                std::stringstream ss;
                ss << frame;
                _lineEditBase->setText(ss.str());
                break;
            }
            default: break;
            }
        }

    } // namespace ViewApp
} // namespace djv

