//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct GroupBox::Private
            {
                std::shared_ptr<Label> label;
                std::shared_ptr<Layout::Stack> childLayout;
                std::shared_ptr<Layout::Vertical> layout;
            };

            void GroupBox::_init(Context * context)
            {
                IContainer::_init(context);

                setClassName("djv::UI::Layout::GroupBox");

                _p->label = Label::create(context);
                _p->label->setFontSizeRole(Style::MetricsRole::FontLarge);
                _p->label->setTextHAlign(TextHAlign::Left);
                _p->label->setMargin(Style::MetricsRole::MarginSmall);
                
                _p->childLayout = Layout::Stack::create(context);

                auto border = Layout::Border::create(context);
                border->addWidget(_p->childLayout);

                _p->layout = Layout::Vertical::create(context);
                _p->layout->setSpacing(Style::MetricsRole::None);
                _p->layout->addWidget(_p->label);
                _p->layout->addWidget(border);
                IContainer::addWidget(_p->layout);
            }

            GroupBox::GroupBox() :
                _p(new Private)
            {}

            GroupBox::~GroupBox()
            {}

            std::shared_ptr<GroupBox> GroupBox::create(Context * context)
            {
                auto out = std::shared_ptr<GroupBox>(new GroupBox);
                out->_init(context);
                return out;
            }

            std::shared_ptr<GroupBox> GroupBox::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<GroupBox>(new GroupBox);
                out->_init(context);
                out->setText(text);
                return out;
            }

            void GroupBox::setText(const std::string & text)
            {
                _p->label->setText(text);
            }
            
            void GroupBox::addWidget(const std::shared_ptr<Widget>& value)
            {
                _p->childLayout->addWidget(value);
            }

            void GroupBox::removeWidget(const std::shared_ptr<Widget>& value)
            {
                _p->childLayout->removeWidget(value);
            }

            void GroupBox::clearWidgets()
            {
                _p->childLayout->clearWidgets();
            }

            float GroupBox::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void GroupBox::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void GroupBox::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
