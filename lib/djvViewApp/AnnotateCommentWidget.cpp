// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotatePrivate.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateCommentWidget::Private
        {
            AV::Time::Units timeUnits = AV::Time::Units::First;
            Math::Frame::Sequence sequence;
            Math::IntRational speed;
            Math::Frame::Index frame = 0;
            std::shared_ptr<UI::Text::Label> authorLabel;
            std::shared_ptr<UI::Text::Label> frameLabel;
            std::shared_ptr<UI::Text::Block> textBlock;
            std::shared_ptr<UI::Layout::Vertical> layout;
            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
        };

        void AnnotateCommentWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setBackgroundColorRole(UI::ColorRole::BackgroundBellows);
            p.authorLabel = UI::Text::Label::create(context);
            p.authorLabel->setFontFace("Bold");
            p.authorLabel->setTextHAlign(UI::TextHAlign::Left);
            p.frameLabel = UI::Text::Label::create(context);
            p.frameLabel->setTextHAlign(UI::TextHAlign::Left);
            p.textBlock = UI::Text::Block::create(context);
            p.layout = UI::Layout::Vertical::create(context);
            p.layout->setMargin(UI::MetricsRole::Margin);
            p.layout->addChild(p.authorLabel);
            p.layout->addChild(p.frameLabel);
            p.layout->addChild(p.textBlock);
            p.layout->setStretch(p.textBlock);
            addChild(p.layout);

            _widgetUpdate();

            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto weak = std::weak_ptr<AnnotateCommentWidget>(std::dynamic_pointer_cast<AnnotateCommentWidget>(shared_from_this()));
            p.timeUnitsObserver = Observer::Value<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        AnnotateCommentWidget::AnnotateCommentWidget() :
            _p(new Private)
        {}

        AnnotateCommentWidget::~AnnotateCommentWidget()
        {}

        std::shared_ptr<AnnotateCommentWidget> AnnotateCommentWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateCommentWidget>(new AnnotateCommentWidget);
            out->_init(context);
            return out;
        }

        void AnnotateCommentWidget::setAuthor(const std::string& value)
        {
            _p->authorLabel->setText(value);
        }

        void AnnotateCommentWidget::setSequence(const Math::Frame::Sequence& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.sequence)
                return;
            p.sequence = value;
            _widgetUpdate();
        }

        void AnnotateCommentWidget::setSpeed(const Math::IntRational& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.speed)
                return;
            p.speed = value;
            _widgetUpdate();
        }

        void AnnotateCommentWidget::setFrame(Math::Frame::Index value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.frame)
                return;
            p.frame = value;
            _widgetUpdate();
        }

        void AnnotateCommentWidget::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void AnnotateCommentWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AnnotateCommentWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void AnnotateCommentWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::string sizeString;
            switch (p.timeUnits)
            {
            case AV::Time::Units::Timecode: sizeString = "00:00:00:00"; break;
            case AV::Time::Units::Frames:   sizeString = "00000"; break;
            default: break;
            }
            p.frameLabel->setSizeString(sizeString);
            std::string text;
            if (p.sequence.getFrameCount() > 1)
            {
                text = AV::Time::toString(p.sequence.getFrame(p.frame), p.speed, p.timeUnits);
            }
            p.frameLabel->setText(text);
        }

    } // namespace ViewApp
} // namespace djv

