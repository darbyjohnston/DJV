// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/TextEdit.h>

#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TextEdit::Private
        {
            std::shared_ptr<TextBlock> textBlock;
            std::shared_ptr<ScrollWidget> scrollWidget;
        };

        void TextEdit::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::TextEdit");
            
            p.textBlock = TextBlock::create(context);
            
            p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
            p.scrollWidget->addChild(p.textBlock);
            addChild(p.scrollWidget);
        }
        
        TextEdit::TextEdit() :
            _p(new Private)
        {}

        TextEdit::~TextEdit()
        {}

        std::shared_ptr<TextEdit> TextEdit::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextEdit>(new TextEdit);
            out->_init(context);
            return out;
        }

        const std::string& TextEdit::getText() const
        {
            return _p->textBlock->getText();
        }

        void TextEdit::setText(const std::string& value)
        {
            _p->textBlock->setText(value);
        }

        void TextEdit::appendText(const std::string& value)
        {
            _p->textBlock->appendText(value);
        }

        void TextEdit::clearText()
        {
            _p->textBlock->clearText();
        }

        MetricsRole TextEdit::getTextSizeRole() const
        {
            return _p->textBlock->getTextSizeRole();
        }

        void TextEdit::setTextSizeRole(MetricsRole value)
        {
            _p->textBlock->setTextSizeRole(value);
        }

        const std::string& TextEdit::getFontFamily() const
        {
            return _p->textBlock->getFontFamily();
        }

        const std::string& TextEdit::getFontFace() const
        {
            return _p->textBlock->getFontFace();
        }

        MetricsRole TextEdit::getFontSizeRole() const
        {
            return _p->textBlock->getFontSizeRole();
        }

        void TextEdit::setFontFamily(const std::string& value)
        {
            _p->textBlock->setFontFamily(value);
        }

        void TextEdit::setFontFace(const std::string& value)
        {
            _p->textBlock->setFontFace(value);
        }

        void TextEdit::setFontSizeRole(MetricsRole value)
        {
            _p->textBlock->setFontSizeRole(value);
        }

        void TextEdit::setBorder(bool value)
        {
            _p->scrollWidget->setBorder(value);
        }
        
        float TextEdit::getHeightForWidth(float value) const
        {
            return _p->scrollWidget->getHeightForWidth(value);
        }

        void TextEdit::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->scrollWidget->getMinimumSize());
        }

        void TextEdit::_layoutEvent(Event::Layout&)
        {
            _p->scrollWidget->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
