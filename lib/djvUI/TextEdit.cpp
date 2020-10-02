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
        namespace Text
        {
            struct Edit::Private
            {
                std::shared_ptr<Block> textBlock;
                std::shared_ptr<ScrollWidget> scrollWidget;
            };

            void Edit::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Text::Edit");

                p.textBlock = Block::create(context);

                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->addChild(p.textBlock);
                addChild(p.scrollWidget);
            }

            Edit::Edit() :
                _p(new Private)
            {}

            Edit::~Edit()
            {}

            std::shared_ptr<Edit> Edit::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Edit>(new Edit);
                out->_init(context);
                return out;
            }

            const std::string& Edit::getText() const
            {
                return _p->textBlock->getText();
            }

            void Edit::setText(const std::string& value)
            {
                _p->textBlock->setText(value);
            }

            void Edit::appendText(const std::string& value)
            {
                _p->textBlock->appendText(value);
            }

            void Edit::clearText()
            {
                _p->textBlock->clearText();
            }

            const std::string& Edit::getFontFamily() const
            {
                return _p->textBlock->getFontFamily();
            }

            const std::string& Edit::getFontFace() const
            {
                return _p->textBlock->getFontFace();
            }

            MetricsRole Edit::getFontSizeRole() const
            {
                return _p->textBlock->getFontSizeRole();
            }

            void Edit::setFontFamily(const std::string& value)
            {
                _p->textBlock->setFontFamily(value);
            }

            void Edit::setFontFace(const std::string& value)
            {
                _p->textBlock->setFontFace(value);
            }

            void Edit::setFontSizeRole(MetricsRole value)
            {
                _p->textBlock->setFontSizeRole(value);
            }

            MetricsRole Edit::getTextSizeRole() const
            {
                return _p->textBlock->getTextSizeRole();
            }

            void Edit::setTextSizeRole(MetricsRole value)
            {
                _p->textBlock->setTextSizeRole(value);
            }

            void Edit::setBorder(bool value)
            {
                _p->scrollWidget->setBorder(value);
            }

            float Edit::getHeightForWidth(float value) const
            {
                return _p->scrollWidget->getHeightForWidth(value);
            }

            void Edit::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void Edit::_layoutEvent(System::Event::Layout&)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }

        } // namespace Text
    } // namespace UI
} // namespace djv
