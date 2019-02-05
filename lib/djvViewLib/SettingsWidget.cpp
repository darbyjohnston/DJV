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

#include <djvViewLib/SettingsWidget.h>

#include <djvViewLib/IViewSystem.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
		struct SettingsWidget::Private
		{
			bool shown = false;
			std::shared_ptr<UI::Button::Group> buttonGroup;
			std::map<std::string, std::string> names;
			std::map<std::string, std::shared_ptr<UI::Widget> > widgets;
			std::map<std::string, std::shared_ptr<UI::Button::List> > buttons;
		};

		void SettingsWidget::_init(Context * context)
		{
			IMDIWidget::_init(MDIResize::Maximum, context);

			DJV_PRIVATE_PTR();
			p.buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);
			for (auto i : context->getSystemsT<IViewSystem>())
			{
				if (auto system = i.lock())
				{
					for (auto widget : system->getSettingsWidgets())
					{
						p.names[widget.sortKey] = widget.name;
						p.widgets[widget.sortKey] = widget.widget;
						auto button = UI::Button::List::create(context);
						p.buttons[widget.sortKey] = button;
					}
				}
			}

			auto buttonLayout = UI::Layout::Vertical::create(context);
			buttonLayout->setSpacing(UI::Style::MetricsRole::None);
			for (auto i : p.buttons)
			{
				p.buttonGroup->addButton(i.second);
				buttonLayout->addWidget(i.second);
			}
			auto buttonScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
			buttonScrollWidget->setBorder(false);
			buttonScrollWidget->addWidget(buttonLayout);

			auto soloLayout = UI::Layout::Solo::create(context);
			for (auto i : p.widgets)
			{
				soloLayout->addWidget(i.second);
			}
			auto soloScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
			soloScrollWidget->setBorder(false);
			soloScrollWidget->addWidget(soloLayout);

			auto layout = UI::Layout::Horizontal::create(context);
			layout->setSpacing(UI::Style::MetricsRole::None);
			layout->addWidget(buttonScrollWidget);
			layout->addWidget(soloScrollWidget, UI::Layout::RowStretch::Expand);
			addWidget(layout);

			auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
			p.buttonGroup->setRadioCallback(
				[soloLayout](int value)
			{
				soloLayout->setCurrentIndex(value);
			});
		}

		SettingsWidget::SettingsWidget() :
			_p(new Private)
		{}

		SettingsWidget::~SettingsWidget()
		{}

		std::shared_ptr<SettingsWidget> SettingsWidget::create(Context * context)
		{
			auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
			out->_init(context);
			return out;
		}

		void SettingsWidget::_localeEvent(Event::Locale &)
		{
			DJV_PRIVATE_PTR();
			setTitle(_getText(DJV_TEXT("djv::ViewLib::SettingsWidget", "Settings")));
			for (const auto & i : p.names)
			{
				const auto j = p.buttons.find(i.first);
				if (j != p.buttons.end())
				{
					j->second->setText(_getText(i.second));
				}
			}
		}

    } // namespace ViewLib
} // namespace djv

