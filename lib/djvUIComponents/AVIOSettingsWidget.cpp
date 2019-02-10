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

#include <djvUIComponents/AVIOSettingsWidget.h>

#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct AVIOSettingsWidget::Private
        {
			std::shared_ptr<VerticalLayout> layout;
        };

        void AVIOSettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
			p.layout = VerticalLayout::create(context);
			p.layout->setParent(shared_from_this());

            auto weak = std::weak_ptr<AVIOSettingsWidget>(std::dynamic_pointer_cast<AVIOSettingsWidget>(shared_from_this()));
        }

		AVIOSettingsWidget::AVIOSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<AVIOSettingsWidget> AVIOSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<AVIOSettingsWidget>(new AVIOSettingsWidget);
            out->_init(context);
            return out;
        }

        float AVIOSettingsWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void AVIOSettingsWidget::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AVIOSettingsWidget::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void AVIOSettingsWidget::_localeEvent(Event::Locale &)
        {
        }

    } // namespace UI
} // namespace djv

