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

#include <djvUIComponents/GeneralSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlatButton.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/FontSettings.h>
#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class DisplaySettingsWidget : public Widget
            {
                DJV_NON_COPYABLE(DisplaySettingsWidget);

            protected:
                void _init(Context *);
                DisplaySettingsWidget();

            public:
                static std::shared_ptr<DisplaySettingsWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _textUpdate();

                DJV_PRIVATE();
            };

            struct DisplaySettingsWidget::Private
            {
                std::vector<std::shared_ptr<FlatButton> > buttons;
                std::shared_ptr<ButtonGroup> buttonGroup;
                std::shared_ptr<FlowLayout> layout;
                std::map<int, std::string> indexToMetrics;
                std::map<std::shared_ptr<FlatButton>, std::string> buttonToMetrics;
                std::map<std::string, int> metricsToIndex;
                std::shared_ptr<MapObserver<std::string, Style::Metrics> > metricsObserver;
                std::shared_ptr<ValueObserver<std::string> > currentMetricsObserver;
            };

            void DisplaySettingsWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

                p.layout = FlowLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);

                auto weak = std::weak_ptr<DisplaySettingsWidget>(std::dynamic_pointer_cast<DisplaySettingsWidget>(shared_from_this()));
                p.buttonGroup->setRadioCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                            {
                                const auto i = widget->_p->indexToMetrics.find(value);
                                if (i != widget->_p->indexToMetrics.end())
                                {
                                    styleSettings->setCurrentMetrics(i->second);
                                }
                            }
                        }
                    }
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
                    if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                    {
                        p.metricsObserver = MapObserver<std::string, Style::Metrics>::create(
                            styleSettings->observeMetrics(),
                            [weak, context](const std::map<std::string, Style::Metrics> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                int currentItem = widget->_p->buttonGroup->getChecked();
                                if (-1 == currentItem && value.size())
                                {
                                    currentItem = 0;
                                }
                                widget->_p->buttons.clear();
                                widget->_p->buttonGroup->clearButtons();
                                widget->_p->layout->clearChildren();
                                widget->_p->indexToMetrics.clear();
                                widget->_p->buttonToMetrics.clear();
                                widget->_p->metricsToIndex.clear();
                                int j = 0;
                                for (const auto & i : value)
                                {
                                    auto button = FlatButton::create(context);
                                    button->setInsideMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                                    widget->_p->buttons.push_back(button);
                                    widget->_p->buttonGroup->addButton(button);
                                    widget->_p->layout->addChild(button);
                                    widget->_p->indexToMetrics[j] = i.first;
                                    widget->_p->buttonToMetrics[button] = i.first;
                                    widget->_p->metricsToIndex[i.first] = j;
                                    ++j;
                                }
                                widget->_p->buttonGroup->setChecked(currentItem);
                                widget->_textUpdate();
                            }
                        });
                        p.currentMetricsObserver = ValueObserver<std::string>::create(
                            styleSettings->observeCurrentMetricsName(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_p->metricsToIndex.find(value);
                                if (i != widget->_p->metricsToIndex.end())
                                {
                                    widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                                }
                            }
                        });
                    }
                }
            }

            DisplaySettingsWidget::DisplaySettingsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<DisplaySettingsWidget> DisplaySettingsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<DisplaySettingsWidget>(new DisplaySettingsWidget);
                out->_init(context);
                return out;
            }

            float DisplaySettingsWidget::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void DisplaySettingsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void DisplaySettingsWidget::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void DisplaySettingsWidget::_localeEvent(Event::Locale & event)
            {
                _textUpdate();
            }

            void DisplaySettingsWidget::_textUpdate()
            {
                DJV_PRIVATE_PTR();
                for (auto i : p.buttons)
                {
                    const auto j = p.buttonToMetrics.find(i);
                    if (j != p.buttonToMetrics.end())
                    {
                        i->setText(_getText(j->second));
                    }
                }
            }

            class PaletteSettingsWidget : public Widget
            {
                DJV_NON_COPYABLE(PaletteSettingsWidget);

            protected:
                void _init(Context *);
                PaletteSettingsWidget();

            public:
                static std::shared_ptr<PaletteSettingsWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _textUpdate();

                DJV_PRIVATE();
            };

            struct PaletteSettingsWidget::Private
            {
                std::vector<std::shared_ptr<FlatButton> > buttons;
                std::shared_ptr<ButtonGroup> buttonGroup;
                std::shared_ptr<FlowLayout> layout;
                std::map<int, std::string> indexToPalette;
                std::map<std::shared_ptr<FlatButton>, std::string> buttonToPalette;
                std::map<std::string, int> paletteToIndex;
                std::shared_ptr<MapObserver<std::string, Style::Palette> > palettesObserver;
                std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
            };

            void PaletteSettingsWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

                p.layout = FlowLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);

                auto weak = std::weak_ptr<PaletteSettingsWidget>(std::dynamic_pointer_cast<PaletteSettingsWidget>(shared_from_this()));
                p.buttonGroup->setRadioCallback(
                    [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<Settings::System>())
                        {
                            if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                            {
                                const auto i = widget->_p->indexToPalette.find(value);
                                if (i != widget->_p->indexToPalette.end())
                                {
                                    styleSettings->setCurrentPalette(i->second);
                                }
                            }
                        }
                    }
                });

                if (auto settingsSystem = context->getSystemT<Settings::System>())
                {
                    if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                    {
                        p.palettesObserver = MapObserver<std::string, Style::Palette>::create(
                            styleSettings->observePalettes(),
                            [weak, context](const std::map<std::string, Style::Palette > & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                int currentItem = widget->_p->buttonGroup->getChecked();
                                if (-1 == currentItem && value.size())
                                {
                                    currentItem = 0;
                                }
                                widget->_p->buttons.clear();
                                widget->_p->buttonGroup->clearButtons();
                                widget->_p->layout->clearChildren();
                                widget->_p->indexToPalette.clear();
                                widget->_p->buttonToPalette.clear();
                                widget->_p->paletteToIndex.clear();
                                int j = 0;
                                for (const auto & i : value)
                                {
                                    auto button = FlatButton::create(context);
                                    button->setInsideMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                                    widget->_p->buttons.push_back(button);
                                    widget->_p->buttonGroup->addButton(button);
                                    widget->_p->layout->addChild(button);
                                    widget->_p->indexToPalette[j] = i.first;
                                    widget->_p->buttonToPalette[button] = i.first;
                                    widget->_p->paletteToIndex[i.first] = j;
                                    ++j;
                                }
                                widget->_p->buttonGroup->setChecked(currentItem);
                                widget->_textUpdate();
                            }
                        });
                        p.currentPaletteObserver = ValueObserver<std::string>::create(
                            styleSettings->observeCurrentPaletteName(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_p->paletteToIndex.find(value);
                                if (i != widget->_p->paletteToIndex.end())
                                {
                                    widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                                }
                            }
                        });
                    }
                }
            }

            PaletteSettingsWidget::PaletteSettingsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<PaletteSettingsWidget> PaletteSettingsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<PaletteSettingsWidget>(new PaletteSettingsWidget);
                out->_init(context);
                return out;
            }

            float PaletteSettingsWidget::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void PaletteSettingsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void PaletteSettingsWidget::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void PaletteSettingsWidget::_localeEvent(Event::Locale & event)
            {
                _textUpdate();
            }

            void PaletteSettingsWidget::_textUpdate()
            {
                DJV_PRIVATE_PTR();
                for (auto i : p.buttons)
                {
                    const auto j = p.buttonToPalette.find(i);
                    if (j != p.buttonToPalette.end())
                    {
                        i->setText(_getText(j->second));
                    }
                }
            }

            class LanguageSettingsWidget : public Widget
            {
                DJV_NON_COPYABLE(LanguageSettingsWidget);

            protected:
                void _init(Context *);
                LanguageSettingsWidget();

            public:
                static std::shared_ptr<LanguageSettingsWidget> create(Context *);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                void _textUpdate();

                DJV_PRIVATE();
            };

            struct LanguageSettingsWidget::Private
            {
                std::vector<std::shared_ptr<FlatButton> > buttons;
                std::shared_ptr<ButtonGroup> buttonGroup;
                std::shared_ptr<FlowLayout> layout;
                std::map<int, std::string> indexToLocale;
                std::map<std::string, int> localeToIndex;
                std::map<std::string, std::string> localeFonts;
                std::map<std::string, std::shared_ptr<FlatButton> > localeToButton;
                std::shared_ptr<ValueObserver<std::string> > localeObserver;
                std::shared_ptr<MapObserver<std::string, std::string> > localeFontsObserver;
            };

            void LanguageSettingsWidget::_init(Context * context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

                p.layout = FlowLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);

                if (auto textSystem = context->getSystemT<TextSystem>())
                {
                    int j = 0;
                    for (const auto & i : textSystem->getLocales())
                    {
                        auto button = FlatButton::create(context);
                        button->setText(_getText(i));
                        button->setInsideMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                        p.buttonGroup->addButton(button);
                        p.layout->addChild(button);
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
                            const auto i = widget->_p->localeToIndex.find(value);
                            if (i != widget->_p->localeToIndex.end())
                            {
                                widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                            }
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

            float LanguageSettingsWidget::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void LanguageSettingsWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void LanguageSettingsWidget::_layoutEvent(Event::Layout &)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void LanguageSettingsWidget::_localeEvent(Event::Locale & event)
            {
                _textUpdate();
            }

            void LanguageSettingsWidget::_textUpdate()
            {
                auto context = getContext();
                if (auto textSystem = context->getSystemT<TextSystem>())
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
                            k->second->setText(_getText(i));
                            k->second->setFont(font);
                        }
                    }
                }
            }

        } // namespace

        struct GeneralSettingsWidget::Private
        {
            std::shared_ptr<GroupBox> displaySizeGroupBox;
            std::shared_ptr<GroupBox> paletteGroupBox;
            std::shared_ptr<GroupBox> languageGroupBox;
        };

        void GeneralSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.displaySizeGroupBox = GroupBox::create(context);
            p.displaySizeGroupBox->addChild(DisplaySettingsWidget::create(context));
            addChild(p.displaySizeGroupBox);

            p.paletteGroupBox = GroupBox::create(context);
            p.paletteGroupBox->addChild(PaletteSettingsWidget::create(context));
            addChild(p.paletteGroupBox);

            p.languageGroupBox = GroupBox::create(context);
            p.languageGroupBox->addChild(LanguageSettingsWidget::create(context));
            addChild(p.languageGroupBox);
        }

        GeneralSettingsWidget::GeneralSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<GeneralSettingsWidget> GeneralSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<GeneralSettingsWidget>(new GeneralSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string GeneralSettingsWidget::getName() const
        {
            return DJV_TEXT("General");
        }

        std::string GeneralSettingsWidget::getSortKey() const
        {
            return "A";
        }

        void GeneralSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.displaySizeGroupBox->setText(_getText(DJV_TEXT("Display Size")));
            p.paletteGroupBox->setText(_getText(DJV_TEXT("Palette")));
            p.languageGroupBox->setText(_getText(DJV_TEXT("Language")));
        }

    } // namespace UI
} // namespace djv

