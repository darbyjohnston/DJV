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

#include <djvUIComponents/LanguageSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/FontSettings.h>
#include <djvUI/ListButton.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LanguageSettingsWidget::Private
        {
            std::vector<std::shared_ptr<ListButton> > buttons;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<FlowLayout> layout;
            std::map<int, std::string> indexToLocale;
            std::map<std::string, int> localeToIndex;
            std::map<std::string, std::string> localeFonts;
            std::map<std::string, std::shared_ptr<ListButton> > localeToButton;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
        };

        void LanguageSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = FlowLayout::create(context);
            addWidget(p.layout);

            if (auto textSystem = context->getSystemT<TextSystem>().lock())
            {
                int j = 0;
                for (const auto & i : textSystem->getLocales())
                {
                    auto button = ListButton::create(context);
                    button->setText(context->getText(i));
                    p.buttonGroup->addButton(button);
                    p.layout->addWidget(button);
                    p.indexToLocale[j] = i;
                    p.localeToIndex[i] = j;
                    p.localeToButton[i] = button;
                    ++j;
                }
            }

            auto weak = std::weak_ptr<LanguageSettingsWidget>(std::dynamic_pointer_cast<LanguageSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto textSystem = context->getSystemT<TextSystem>().lock())
                    {
                        const auto i = widget->_p->indexToLocale.find(value);
                        if (i != widget->_p->indexToLocale.end())
                        {
                            textSystem->setCurrentLocale(i->second);
                        }
                    }
                }
            });

            if (auto textSystem = context->getSystemT<TextSystem>().lock())
            {
                p.localeObserver = ValueObserver<std::string>::create(
                    textSystem->observeCurrentLocale(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->localeToIndex.find(value);
                        if (i != widget->_p->localeToIndex.end())
                        {
                            widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                        }
                    }
                });
            }

			if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
			{
				if (auto fontSettings = settingsSystem->getSettingsT<Settings::Font>())
				{
					p.localeFontsObserver = MapObserver<std::string, std::string>::create(
						fontSettings->observeLocaleFonts(),
						[weak](const std::map<std::string, std::string> & value)
					{
						if (auto widget = weak.lock())
						{
							widget->_p->localeFonts = value;
							widget->_textUpdate();
						}
					});
				}
            }
        }

        LanguageSettingsWidget::LanguageSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<LanguageSettingsWidget> LanguageSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<LanguageSettingsWidget>(new LanguageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string LanguageSettingsWidget::getName() const
        {
            return DJV_TEXT("djv::UI::LanguageSettingsWidget", "Language");
        }

        std::string LanguageSettingsWidget::getGroup() const
        {
            return DJV_TEXT("djv::UI::Settings", "General");
        }

        std::string LanguageSettingsWidget::getGroupSortKey() const
        {
            return "A";
        }

        void LanguageSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            _textUpdate();
        }

        void LanguageSettingsWidget::_textUpdate()
        {
            auto context = getContext();
            if (auto textSystem = context->getSystemT<TextSystem>().lock())
            {
                for (const auto & i : textSystem->getLocales())
                {
                    std::string font;
                    auto j = _p->localeFonts.find(i);
                    if (j != _p->localeFonts.end())
                    {
                        font = j->second;
                    }
                    else
                    {
                        j = _p->localeFonts.find("Default");
                        if (j != _p->localeFonts.end())
                        {
                            font = j->second;
                        }
                    }
                    const auto k = _p->localeToButton.find(i);
                    if (k != _p->localeToButton.end())
                    {
                        k->second->setText(context->getText(i));
                        k->second->setFont(font);
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv

