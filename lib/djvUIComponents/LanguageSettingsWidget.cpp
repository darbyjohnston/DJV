// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void LanguageWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.comboBox->clearItems();
                auto textSystem = _getTextSystem();
                const auto& locales = textSystem->getLocales();
                std::vector<std::string> items;
                std::vector<std::pair<int, std::string> > fonts;
                int j = 0;
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
                    items.push_back(_getText(i));
                    fonts.push_back(std::make_pair(j, font));
                    p.indexToLocale[j] = i;
                    p.localeToIndex[i] = j;
                    ++j;
                }
                const auto i = p.localeToIndex.find(p.locale);
                if (i != p.localeToIndex.end())
                {
                    p.comboBox->setItems(items);
                    p.comboBox->setCurrentItem(static_cast<int>(i->second));
                    for (const auto k : fonts)
                    {
                        p.comboBox->setFont(k.first, k.second);
                    }
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

