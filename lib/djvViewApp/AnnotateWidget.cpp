// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotatePrivate.h>

#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateWidget::Private
        {
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void AnnotateWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::ViewApp::AnnotateWidget");
            
            /*auto summaryWidget = AnnotateSummaryWidget::create(context);
            summaryWidget->setAuthor("Author");
            summaryWidget->setText("This is the summary");

            auto commentWidget = AnnotateCommentWidget::create(context);
            commentWidget->setAuthor("Author");
            commentWidget->setSequence(Math::Frame::Sequence(0, 1000));
            commentWidget->setSpeed(Math::IntRational(24, 1));
            commentWidget->setFrame(100);
            commentWidget->setText("This is a comment");*/

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::Border);
            //p.layout->addChild(summaryWidget);
            //p.layout->addChild(commentWidget);
            addChild(p.layout);
        }

        AnnotateWidget::AnnotateWidget() :
            _p(new Private)
        {}

        AnnotateWidget::~AnnotateWidget()
        {}

        std::shared_ptr<AnnotateWidget> AnnotateWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateWidget>(new AnnotateWidget);
            out->_init(context);
            return out;
        }

        void AnnotateWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AnnotateWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace ViewApp
} // namespace djv

