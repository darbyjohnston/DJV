// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotatePrivate.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSummaryWidget::Private
        {
            std::shared_ptr<UI::Text::Label> authorLabel;
            std::shared_ptr<UI::Text::Block> textBlock;
            std::shared_ptr<UI::Layout::Vertical> layout;
        };

        void AnnotateSummaryWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setBackgroundColorRole(UI::ColorRole::BackgroundBellows);
            p.authorLabel = UI::Text::Label::create(context);
            p.authorLabel->setFontFace("Bold");
            p.authorLabel->setTextHAlign(UI::TextHAlign::Left);
            p.textBlock = UI::Text::Block::create(context);
            p.layout = UI::Layout::Vertical::create(context);
            p.layout->setMargin(UI::MetricsRole::Margin);
            p.layout->addChild(p.authorLabel);
            p.layout->addChild(p.textBlock);
            p.layout->setStretch(p.textBlock);
            addChild(p.layout);
        }

        AnnotateSummaryWidget::AnnotateSummaryWidget() :
            _p(new Private)
        {}

        AnnotateSummaryWidget::~AnnotateSummaryWidget()
        {}

        std::shared_ptr<AnnotateSummaryWidget> AnnotateSummaryWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSummaryWidget>(new AnnotateSummaryWidget);
            out->_init(context);
            return out;
        }

        void AnnotateSummaryWidget::setAuthor(const std::string& value)
        {
            _p->authorLabel->setText(value);
        }

        void AnnotateSummaryWidget::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void AnnotateSummaryWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AnnotateSummaryWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace ViewApp
} // namespace djv

