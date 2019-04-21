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

#include <djvUIComponents/LanguageSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FontSettings.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
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
            std::map<size_t, std::string> indexToLocale;
            std::map<std::string, size_t> localeToIndex;
            std::map<std::string, std::string> localeFonts;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
        };

        void LanguageWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            auto weak = std::weak_ptr<LanguageWidget>(std::dynamic_pointer_cast<LanguageWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto textSystem = context->getSystemT<TextSystem>())
                    {
                        const auto i = widget->_p->indexToLocale.find(value);
                        if (i != widget->_p->indexToLocale.end())
                        {
                            textSystem->setCurrentLocale(i->second);
                        }
                    }
                }
            });

            if (auto textSystem = context->getSystemT<TextSystem>())
            {
                p.localeObserver = ValueObserver<std::string>::create(
                    textSystem->observeCurrentLocale(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->locale = value;
                        widget->_currentItemUpdate();
                    }
                });
            }

            if (auto settingsSystem = context->getSystemT<Settings::System>())
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
                            widget->_widgetUpdate();
                        }
                    });
                }
            }
        }

        LanguageWidget::LanguageWidget() :
            _p(new Private)
        {}

        std::shared_ptr<LanguageWidget> LanguageWidget::create(Context* context)
        {
            auto out = std::shared_ptr<LanguageWidget>(new LanguageWidget);
            out->_init(context);
            return out;
        }

        void LanguageWidget::setFontSizeRole(UI::MetricsRole value)
        {
            _p->comboBox->setFontSizeRole(value);
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

        void LanguageWidget::_localeEvent(Event::Locale& event)
        {
            _widgetUpdate();
        }

        void LanguageWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            if (auto textSystem = context->getSystemT<TextSystem>())
            {
                p.comboBox->clearItems();
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
                    p.comboBox->setFont(static_cast<int>(j), font);
                    p.indexToLocale[j] = i;
                    p.localeToIndex[i] = j;
                    ++j;
                }
                _currentItemUpdate();
            }
        }

        void LanguageWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.localeToIndex.find(p.locale);
            if (i != p.localeToIndex.end())
            {
                p.comboBox->setCurrentItem(static_cast<int>(i->second));
            }
        }

        struct LanguageSettingsWidget::Private
        {
            std::shared_ptr<LanguageWidget> languageWidget;
            std::shared_ptr<FormLayout> layout;
        };

        void LanguageSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.languageWidget = LanguageWidget::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.languageWidget);
            addChild(p.layout);
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

        std::string LanguageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Language");
        }

        std::string LanguageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string LanguageSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void LanguageSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.languageWidget, _getText(DJV_TEXT("Language:")));
        }

    } // namespace UI
} // namespace djv

