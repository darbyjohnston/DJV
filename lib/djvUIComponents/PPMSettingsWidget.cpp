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

#include <djvUIComponents/PPMSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/ListButton.h>

#include <djvAV/PPM.h>
#include <djvAV/PPMJSON.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
		struct PPMSettingsWidget::Private
		{
			std::shared_ptr<ButtonGroup> dataButtonGroup;
			std::shared_ptr<GroupBox> dataGroupBox;
		};

		void PPMSettingsWidget::_init(Context * context)
		{
            ISettingsWidget::_init(context);

			DJV_PRIVATE_PTR();

			p.dataButtonGroup = ButtonGroup::create(ButtonType::Radio);
			auto flowLayout = FlowLayout::create(context);
			for (size_t i = 0; i < static_cast<size_t>(AV::IO::PPM::Data::Count); ++i)
			{
				auto button = ListButton::create(context);
				p.dataButtonGroup->addButton(button);
				flowLayout->addWidget(button);
			}
			p.dataGroupBox = GroupBox::create(context);
			p.dataGroupBox->addWidget(flowLayout);
			addWidget(p.dataGroupBox);

			if (auto io = context->getSystemT<AV::IO::System>().lock())
			{
				AV::IO::PPM::Settings settings;
				fromJSON(io->getOptions(AV::IO::PPM::pluginName), settings);
				p.dataButtonGroup->setChecked(static_cast<int>(settings.data));
			}

			auto weak = std::weak_ptr<PPMSettingsWidget>(std::dynamic_pointer_cast<PPMSettingsWidget>(shared_from_this()));
			p.dataButtonGroup->setRadioCallback(
				[weak, context](int value)
			{
				if (auto io = context->getSystemT<AV::IO::System>().lock())
				{
					AV::IO::PPM::Settings settings;
					settings.data = static_cast<AV::IO::PPM::Data>(value);
					io->setOptions(AV::IO::PPM::pluginName, toJSON(settings));
				}
			});
		}

		PPMSettingsWidget::PPMSettingsWidget() :
			_p(new Private)
		{}

		std::shared_ptr<PPMSettingsWidget> PPMSettingsWidget::create(Context * context)
		{
			auto out = std::shared_ptr<PPMSettingsWidget>(new PPMSettingsWidget);
			out->_init(context);
			return out;
		}

        std::string PPMSettingsWidget::getName() const
        {
            return DJV_TEXT("djv::UI::PPMSettingsWidget", "PPM");
        }

        std::string PPMSettingsWidget::getGroup() const
        {
            return DJV_TEXT("djv::UI::Settings", "IO");
        }

        std::string PPMSettingsWidget::getGroupSortKey() const
        {
            return DJV_TEXT("djv::UI::Settings", "B");
        }

		void PPMSettingsWidget::_localeEvent(Event::Locale & event)
		{
            ISettingsWidget::_localeEvent(event);
			DJV_PRIVATE_PTR();
			const auto & buttons = p.dataButtonGroup->getButtons();
			for (size_t i = 0; i < buttons.size(); ++i)
			{
				if (auto button = std::dynamic_pointer_cast<ListButton>(buttons[i]))
				{
					std::stringstream ss;
					ss << static_cast<AV::IO::PPM::Data>(i);
					button->setText(_getText(ss.str()));
				}
			}
			p.dataGroupBox->setText(_getText(DJV_TEXT("djv::UI::PPMSettingsWidget", "Data Type")));
		}

    } // namespace UI
} // namespace djv

