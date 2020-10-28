// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/LanguageSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FontSettings.h>
#include <djvUI/FormLayout.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvAV/AVSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct LanguageWidget::Private
        {
            std::string locale;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::map<int, std::string> indexToLocale;
            std::map<std::string, int> localeToIndex;
            std::map<std::string, std::string> localeFonts;
            std::shared_ptr<Observer::Value<std::string> > localeObserver;
            std::shared_ptr<Observer::Map<std::string, std::string> > localeFontsObserver;
        };

        void LanguageWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::LanguageWidget");
            setHAlign(UI::HAlign::Fill);

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<LanguageWidget>(std::dynamic_pointer_cast<LanguageWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto textSystem = context->getSystemT<System::TextSystem>();
                            const auto i = widget->_p->indexToLocale.find(value);
                            if (i != widget->_p->indexToLocale.end())
                            {
                                textSystem->setCurrentLocale(i->second);
                            }
                        }
                    }
                });

            auto textSystem = context->getSystemT<System::TextSystem>();
            p.localeObserver = Observer::Value<std::string>::create(
                textSystem->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->locale = value;
                    widget->_widgetUpdate();
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fontSettings = settingsSystem->getSettingsT<UI::Settings::Font>();
            p.localeFontsObserver = Observer::Map<std::string, std::string>::create(
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

        std::shared_ptr<LanguageWidget> LanguageWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<LanguageWidget>(new LanguageWidget);
            out->_init(context);
            return out;
        }

        float LanguageWidget::getHeightForWidth(float value) const
        {
            return _p->comboBox->getHeightForWidth(value);
        }

        void LanguageWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void LanguageWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void LanguageWidget::_initEvent(System::Event::Init& event)
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

        namespace Settings
        {
            struct LanguageWidget::Private
            {
                std::shared_ptr<UIComponents::LanguageWidget> languageWidget;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void LanguageWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::LanguageWidget");

                p.languageWidget = UIComponents::LanguageWidget::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.languageWidget);
                addChild(p.layout);
            }

            LanguageWidget::LanguageWidget() :
                _p(new Private)
            {}

            std::shared_ptr<LanguageWidget> LanguageWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<LanguageWidget>(new LanguageWidget);
                out->_init(context);
                return out;
            }

            std::string LanguageWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_general");
            }

            std::string LanguageWidget::getSettingsSortKey() const
            {
                return "0";
            }

            void LanguageWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
            {
                _p->layout->setLabelSizeGroup(value);
            }

            void LanguageWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.languageWidget, _getText(DJV_TEXT("settings_language")) + ":");
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

