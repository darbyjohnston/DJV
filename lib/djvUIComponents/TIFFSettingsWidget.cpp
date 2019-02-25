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

#include <djvUIComponents/TIFFSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlatButton.h>
#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>

#include <djvAV/TIFF.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
		struct TIFFSettingsWidget::Private
		{
			std::shared_ptr<ButtonGroup> compressionButtonGroup;
			std::shared_ptr<GroupBox> compressionGroupBox;
		};

		void TIFFSettingsWidget::_init(Context * context)
		{
            ISettingsWidget::_init(context);

			DJV_PRIVATE_PTR();

			p.compressionButtonGroup = ButtonGroup::create(ButtonType::Radio);
			auto layout = VerticalLayout::create(context);
            layout->setSpacing(MetricsRole::None);
			for (size_t i = 0; i < static_cast<size_t>(AV::IO::TIFF::Compression::Count); ++i)
			{
				auto button = FlatButton::create(context);
				p.compressionButtonGroup->addButton(button);
				layout->addWidget(button);
			}
			p.compressionGroupBox = GroupBox::create(context);
			p.compressionGroupBox->addWidget(layout);
			addWidget(p.compressionGroupBox);

			if (auto io = context->getSystemT<AV::IO::System>().lock())
			{
				AV::IO::TIFF::Settings settings;
				fromJSON(io->getOptions(AV::IO::TIFF::pluginName), settings);
				p.compressionButtonGroup->setChecked(static_cast<int>(settings.compression));
			}

			auto weak = std::weak_ptr<TIFFSettingsWidget>(std::dynamic_pointer_cast<TIFFSettingsWidget>(shared_from_this()));
			p.compressionButtonGroup->setRadioCallback(
				[weak, context](int value)
			{
				if (auto io = context->getSystemT<AV::IO::System>().lock())
				{
					AV::IO::TIFF::Settings settings;
					settings.compression = static_cast<AV::IO::TIFF::Compression>(value);
					io->setOptions(AV::IO::TIFF::pluginName, toJSON(settings));
				}
			});
		}

        TIFFSettingsWidget::TIFFSettingsWidget() :
			_p(new Private)
		{}

		std::shared_ptr<TIFFSettingsWidget> TIFFSettingsWidget::create(Context * context)
		{
			auto out = std::shared_ptr<TIFFSettingsWidget>(new TIFFSettingsWidget);
			out->_init(context);
			return out;
		}

        std::string TIFFSettingsWidget::getName() const
        {
            return DJV_TEXT("TIFF");
        }

        std::string TIFFSettingsWidget::getGroup() const
        {
            return DJV_TEXT("I/O");
        }

        std::string TIFFSettingsWidget::getGroupSortKey() const
        {
            return "B";
        }

		void TIFFSettingsWidget::_localeEvent(Event::Locale & event)
		{
            ISettingsWidget::_localeEvent(event);
			DJV_PRIVATE_PTR();
			const auto & buttons = p.compressionButtonGroup->getButtons();
			for (size_t i = 0; i < buttons.size(); ++i)
			{
				if (auto button = std::dynamic_pointer_cast<FlatButton>(buttons[i]))
				{
					std::stringstream ss;
					ss << static_cast<AV::IO::TIFF::Compression>(i);
					button->setText(_getText(ss.str()));
				}
			}
			p.compressionGroupBox->setText(_getText(DJV_TEXT("File Compression")));
		}

    } // namespace UI
} // namespace djv

