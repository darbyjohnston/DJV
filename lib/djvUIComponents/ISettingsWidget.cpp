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

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ISettingsWidget::Private
        {
            std::shared_ptr<VerticalLayout> childLayout;
        };

        void ISettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ISettingsWidget");

            p.childLayout = VerticalLayout::create(context);
            //p.childLayout->setMargin(MetricsRole::MarginLarge);
            //p.childLayout->setSpacing(MetricsRole::SpacingLarge);
            Widget::addChild(p.childLayout);
        }

        ISettingsWidget::ISettingsWidget() :
            _p(new Private)
        {}

        ISettingsWidget::~ISettingsWidget()
        {}

        float ISettingsWidget::getHeightForWidth(float value) const
        {
            return _p->childLayout->getHeightForWidth(value);
        }

        void ISettingsWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->addChild(value);
        }

        void ISettingsWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void ISettingsWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->childLayout->getMinimumSize());
        }

        void ISettingsWidget::_layoutEvent(Event::Layout &)
        {
            _p->childLayout->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv

