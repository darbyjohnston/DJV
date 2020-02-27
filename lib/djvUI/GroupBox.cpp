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

#include <djvUI/GroupBox.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct GroupBox::Private
            {
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<StackLayout> childLayout;
                std::shared_ptr<VerticalLayout> layout;
            };

            void GroupBox::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::GroupBox");
                setVAlign(VAlign::Top);

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(TextHAlign::Left);
                p.titleLabel->setFontFace("Bold");
                p.titleLabel->setMargin(Layout::Margin(MetricsRole::MarginSmall));

                p.childLayout = StackLayout::create(context);
                p.childLayout->setMargin(Layout::Margin(MetricsRole::MarginSmall));

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                p.layout->addChild(p.titleLabel);
                p.layout->addSeparator();
                p.layout->addChild(p.childLayout);
                p.layout->setStretch(p.childLayout, RowStretch::Expand);
                Widget::addChild(p.layout);
            }

            GroupBox::GroupBox() :
                _p(new Private)
            {}

            GroupBox::~GroupBox()
            {}

            std::shared_ptr<GroupBox> GroupBox::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<GroupBox>(new GroupBox);
                out->_init(context);
                return out;
            }

            const std::string & GroupBox::getText() const
            {
                return _p->titleLabel->getText();
            }

            void GroupBox::setText(const std::string & text)
            {
                _p->titleLabel->setText(text);
            }

            float GroupBox::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void GroupBox::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->addChild(value);
            }

            void GroupBox::removeChild(const std::shared_ptr<IObject>& value)
            {
                _p->childLayout->removeChild(value);
            }

            void GroupBox::clearChildren()
            {
                _p->childLayout->clearChildren();
            }

            void GroupBox::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void GroupBox::_layoutEvent(Event::Layout & event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
