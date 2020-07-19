// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidgetPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/NumericEdit.h>
#include <djvUI/RowLayout.h>

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
        struct FrameWidget::Private
        {
            Core::Time::Units timeUnits = Core::Time::Units::First;
            Core::Frame::Sequence sequence;
            Core::Math::Rational speed;
            Core::Frame::Index index = 0;
            std::shared_ptr<UI::LineEditBase> lineEditBase;
            std::shared_ptr<UI::NumericEditButtons> buttons;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::function<void(Core::Frame::Index)> callback;
            std::shared_ptr<Core::ValueObserver<Core::Time::Units> > timeUnitsObserver;
        };

        void FrameWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::FrameWidget");
            setVAlign(UI::VAlign::Center);

            auto nextAction = UI::Action::create();
            nextAction->setShortcut(GLFW_KEY_UP);
            auto prevAction = UI::Action::create();
            prevAction->setShortcut(GLFW_KEY_DOWN);
            auto nextX10Action = UI::Action::create();
            nextX10Action->setShortcut(GLFW_KEY_PAGE_UP);
            auto prevX10Action = UI::Action::create();
            prevX10Action->setShortcut(GLFW_KEY_PAGE_DOWN);

            p.lineEditBase = UI::LineEditBase::create(context);
            p.lineEditBase->setFont(AV::Font::familyMono);
            p.lineEditBase->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.lineEditBase->setBackgroundRole(UI::ColorRole::None);
            p.lineEditBase->addAction(nextAction);
            p.lineEditBase->addAction(prevAction);
            p.lineEditBase->addAction(nextX10Action);
            p.lineEditBase->addAction(prevX10Action);

            p.buttons = UI::NumericEditButtons::create(context);
            p.buttons->setBackgroundRole(UI::ColorRole::None);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.lineEditBase);
            p.layout->setStretch(p.lineEditBase, UI::RowStretch::Expand);
            p.layout->addChild(p.buttons);
            addChild(p.layout);
            
            _widgetUpdate();
            
            auto weak = std::weak_ptr<FrameWidget>(std::dynamic_pointer_cast<FrameWidget>(shared_from_this()));
            p.lineEditBase->setTextEditCallback(
                [weak](const std::string& value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        Frame::Index index = Frame::invalidIndex;
                        try
                        {
                            index = widget->_p->sequence.getIndex(Time::fromString(value, widget->_p->speed, widget->_p->timeUnits));
                        }
                        catch (const std::exception&)
                        {
                            //! \todo How can we translate this?
                            widget->_log(widget->_getText(DJV_TEXT("error_cannot_parse_the_value")), LogLevel::Error);
                        }
                        widget->_setFrame(index);
                    }
                });
            p.lineEditBase->setFocusCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_redraw();
                    }
                });

            p.buttons->setIncrementCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index + 1);
                    }
                });
            p.buttons->setDecrementCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index - 1);
                    }
                });

            nextAction->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index + 1);
                    }
                });

            prevAction->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index - 1);
                    }
                });
            nextX10Action->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index + 10);
                    }
                });
            prevX10Action->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setFrame(widget->_p->index - 10);
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        FrameWidget::FrameWidget() :
            _p(new Private)
        {}

        std::shared_ptr<FrameWidget> FrameWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FrameWidget>(new FrameWidget);
            out->_init(context);
            return out;
        }

        void FrameWidget::setSequence(const Frame::Sequence& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sequence)
                return;
            p.sequence = value;
            _widgetUpdate();
        }

        void FrameWidget::setSpeed(const Math::Rational& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.speed)
                return;
            p.speed = value;
            _widgetUpdate();
        }

        void FrameWidget::setFrame(const Frame::Index value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.index)
                return;
            p.index = value;
            _widgetUpdate();
        }

        void FrameWidget::setCallback(const std::function<void(Frame::Index)>& value)
        {
            _p->callback = value;
        }

        void FrameWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            _setMinimumSize(_p->layout->getMinimumSize() + btf * 2.F);
        }

        void FrameWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            _p->layout->setGeometry(getGeometry().margin(-btf));
        }

        void FrameWidget::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float btf = style->getMetric(UI::MetricsRole::BorderTextFocus);
            const auto& render = _getRender();
            if (p.lineEditBase->hasTextFocus())
            {
                render->setFillColor(style->getColor(UI::ColorRole::TextFocus));
                UI::drawBorder(render, g, btf);
            }
            else
            {
                render->setFillColor(style->getColor(UI::ColorRole::Border));
                UI::drawBorder(render, g.margin(-b), b);
            }
        }

        void FrameWidget::_setFrame(Frame::Index value)
        {
            DJV_PRIVATE_PTR();
            if (value != Frame::invalidIndex)
            {
                p.index = value;
                if (p.callback)
                {
                    p.callback(p.index);
                }
            }
            _widgetUpdate();
        }

        void FrameWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::string sizeString;
            switch (p.timeUnits)
            {
            case Time::Units::Timecode: sizeString = "00:00:00:00"; break;
            case Time::Units::Frames:   sizeString = "00000"; break;
            default: break;
            }
            p.lineEditBase->setSizeString(sizeString);
            p.lineEditBase->setText(Time::toString(p.sequence.getFrame(p.index), p.speed, p.timeUnits));
        }

    } // namespace ViewApp
} // namespace djv

