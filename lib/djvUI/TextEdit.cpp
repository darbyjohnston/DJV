//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

        const std::string & TextEdit::getText() const
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

        const std::string & TextEdit::getFontFamily() const
        {
            return _p->textBlock->getFontFamily();
        }

        const std::string & TextEdit::getFontFace() const
        {
            return _p->textBlock->getFontFace();
        }

        MetricsRole TextEdit::getFontSizeRole() const
        {
            return _p->textBlock->getFontSizeRole();
        }

        void TextEdit::setFontFamily(const std::string & value)
        {
            _p->textBlock->setFontFamily(value);
        }

        void TextEdit::setFontFace(const std::string & value)
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
