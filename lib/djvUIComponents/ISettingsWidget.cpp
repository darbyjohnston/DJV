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

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ISettingsWidget::Private
        {
            std::shared_ptr<StackLayout> layout;
        };

        void ISettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            p.layout = StackLayout::create(context);
            p.layout->setMargin(MetricsRole::MarginLarge);
            IContainer::addWidget(p.layout);
        }

        ISettingsWidget::ISettingsWidget() :
            _p(new Private)
        {}

        ISettingsWidget::~ISettingsWidget()
        {}

        void ISettingsWidget::addWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->layout->addWidget(widget);
        }

        void ISettingsWidget::removeWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->layout->addWidget(widget);
        }

        void ISettingsWidget::clearWidgets()
        {
            _p->layout->clearWidgets();
        }

        float ISettingsWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void ISettingsWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ISettingsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv

