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

#include <djvUIComponents/LanguageSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FontSettings.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct LanguageWidget::Private
        {
            std::string locale;
            std::shared_ptr<ComboBox> comboBox;
            std::map<int, std::string> indexToLocale;
            std::map<std::string, int> localeToIndex;
            std::map<std::string, std::string> localeFonts;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
        };

        void LanguageWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::LanguageWidget");
            setHAlign(HAlign::Left);

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<LanguageWidget>(std::dynamic_pointer_cast<LanguageWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto textSystem = context->getSystemT<TextSystem>();
                            const auto i = widget->_p->indexToLocale.find(value);
                            if (i != widget->_p->indexToLocale.end())
                            {
                                textSystem->setCurrentLocale(i->second);
                            }
                        }
                    }
                });

            auto textSystem = context->getSystemT<TextSystem>();
            p.localeObserver = ValueObserver<std::string>::create(
                textSystem->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->locale = value;
                    widget->_widgetUpdate();
                }
            });

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto fontSettings = settingsSystem->getSettingsT<Settings::Font>();
            p.localeFontsObserver = MapObserver<std::string, std::string>::create(
                fontSettings->observeLocaleFonts(),
                [weak](const std::map<std::string, std::string> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->localeFonts = value;
                    widget->_widgetUpdate();
                }
            });
        }

        LanguageWidget::LanguageWidget() :
            _p(new Private)
        {}

        std::shared_ptr<LanguageWidget> LanguageWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LanguageWidget>(new LanguageWidget);
            out->_init(context);
            return out;
        }

        float LanguageWidget::getHeightForWidth(float value) const
        {
            return _p->comboBox->getHeightForWidth(value);
        }

        void LanguageWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void LanguageWidget::_layoutEvent(Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void LanguageWidget::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            _widgetUpdate();
        }

        void LanguageWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.comboBox->clearItems();
                auto textSystem = _getTextSystem();
                const auto& locales = textSystem->getLocales();
                size_t j = 0;
                for (const auto& i : locales)
                {
                    std::string font;
                    auto k = p.localeFonts.find(i);
                    if (k != p.localeFonts.end())
                    {
                        font = k->second;
                    }
                    else
                    {
                        k = p.localeFonts.find("Default");
                        if (k != p.localeFonts.end())
                        {
                            font = k->second;
                        }
                    }
                    p.comboBox->addItem(_getText(i));
                    p.comboBox->setFont(j, font);
                    p.indexToLocale[j] = i;
                    p.localeToIndex[i] = j;
                    ++j;
                }
                const auto i = p.localeToIndex.find(p.locale);
                if (i != p.localeToIndex.end())
                {
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                }
            }
        }

        struct LanguageSettingsWidget::Private
        {
            std::shared_ptr<LanguageWidget> languageWidget;
        };

        void LanguageSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::LanguageSettingsWidget");

            p.languageWidget = LanguageWidget::create(context);
            addChild(p.languageWidget);
        }

        LanguageSettingsWidget::LanguageSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<LanguageSettingsWidget> LanguageSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<LanguageSettingsWidget>(new LanguageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string LanguageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_language");
        }

        std::string LanguageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string LanguageSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

    } // namespace UI
} // namespace djv

