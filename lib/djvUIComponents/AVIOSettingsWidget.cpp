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

#include <djvUIComponents/PPMSettingsWidget.h>
#if defined(JPEG_FOUND)
#include <djvUIComponents/JPEGSettingsWidget.h>
#endif // JPEG_FOUND

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/Splitter.h>

#include <djvAV/PPM.h>
#if defined(JPEG_FOUND)
#include <djvAV/JPEG.h>
#endif // JPEG_FOUND

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct AVIOSettingsWidget::Private
        {
			std::map<std::string, std::shared_ptr<Widget> > widgets;
			std::map<std::string, std::shared_ptr<ListButton> > buttons;
			std::shared_ptr<ButtonGroup> buttonGroup;
			std::shared_ptr<Layout::Splitter> splitter;
        };

        void AVIOSettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
			p.widgets[AV::IO::PPM::pluginName] = PPMSettingsWidget::create(context);
#if defined(JPEG_FOUND)
			p.widgets[AV::IO::JPEG::pluginName] = JPEGSettingsWidget::create(context);
#endif // JPEG_FOUND

			p.buttonGroup = ButtonGroup::create(ButtonType::Radio);
			for (auto i : p.widgets)
			{
				auto button = ListButton::create(context);
				p.buttons[i.first] = button;
			}

			auto buttonLayout = VerticalLayout::create(context);
			buttonLayout->setSpacing(MetricsRole::None);
			for (auto i : p.buttons)
			{
				p.buttonGroup->addButton(i.second);
				buttonLayout->addWidget(i.second);
			}
			auto buttonScrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
			buttonScrollWidget->setBorder(false);
			buttonScrollWidget->addWidget(buttonLayout);

			auto soloLayout = SoloLayout::create(context);
			for (auto i : p.widgets)
			{
				soloLayout->addWidget(i.second);
			}
			auto soloScrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
			soloScrollWidget->setBorder(false);
			soloScrollWidget->addWidget(soloLayout);

			p.splitter = Layout::Splitter::create(Orientation::Horizontal, context);
			p.splitter->setSplit(.15f);
			p.splitter->addWidget(buttonScrollWidget);
			p.splitter->addWidget(soloScrollWidget);
			p.splitter->setParent(shared_from_this());

            auto weak = std::weak_ptr<AVIOSettingsWidget>(std::dynamic_pointer_cast<AVIOSettingsWidget>(shared_from_this()));;
			p.buttonGroup->setRadioCallback(
				[soloLayout](int value)
			{
				soloLayout->setCurrentIndex(value);
			});
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
            return _p->splitter->getHeightForWidth(value);
        }

        void AVIOSettingsWidget::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(_p->splitter->getMinimumSize());
        }

        void AVIOSettingsWidget::_layoutEvent(Event::Layout &)
        {
            _p->splitter->setGeometry(getGeometry());
        }

        void AVIOSettingsWidget::_localeEvent(Event::Locale &)
        {
			DJV_PRIVATE_PTR();
			for (const auto & i : p.buttons)
			{
				i.second->setText(_getText(i.first));
			}
        }

    } // namespace UI
} // namespace djv

