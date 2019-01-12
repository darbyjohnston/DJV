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

#include <djvViewLib/SettingsSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/ComboBox.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/ToolButton.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            class SettingsWidget : public UI::Layout::Vertical
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(Context * context)
                {
                    Vertical::_init(context);

                    setSpacing(UI::Style::MetricsRole::None);
                    setBackgroundRole(UI::Style::ColorRole::Background);
                    setPointerEnabled(true);

                    auto titleIcon = UI::Icon::create(context);
                    titleIcon->setIcon("djvIconSettings");
                    titleIcon->setMargin(UI::Layout::Margin(
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::None,
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::MarginSmall));
                    
                    _titleLabel = UI::Label::create(context);
                    _titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    _titleLabel->setTextHAlign(UI::TextHAlign::Left);
                    _titleLabel->setMargin(UI::Layout::Margin(
                        UI::Style::MetricsRole::Margin,
                        UI::Style::MetricsRole::None,
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::MarginSmall));
                    
                    _closeButton = UI::Button::Tool::create(context);
                    _closeButton->setIcon("djvIconClose");
                    _closeButton->setInsideMargin(UI::Style::MetricsRole::MarginSmall);
                    
                    std::map<std::string, std::shared_ptr<UI::Widget> > settingsWidgets;
                    for (auto i : context->getSystemsT<IViewSystem>())
                    {
                        if (auto system = i.lock())
                        {
                            for (auto j : system->createSettingsWidgets())
                            {
                                settingsWidgets[j.sortKey] = j.widget;
                            }
                        }
                    }
                    auto settingsLayout = UI::Layout::Vertical::create(context);
                    settingsLayout->setMargin(UI::Style::MetricsRole::Margin);
                    for (auto i : settingsWidgets)
                    {
                        settingsLayout->addWidget(i.second);
                    }
                    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                    scrollWidget->setBorder(false);
                    scrollWidget->setBackgroundRole(UI::Style::ColorRole::Background);
                    scrollWidget->addWidget(settingsLayout);
                    
                    auto hLayout = UI::Layout::Horizontal::create(context);
                    hLayout->setSpacing(UI::Style::MetricsRole::None);
                    hLayout->setBackgroundRole(UI::Style::ColorRole::BackgroundHeader);
                    hLayout->addWidget(titleIcon);
                    hLayout->addWidget(_titleLabel, UI::Layout::RowStretch::Expand);
                    hLayout->addWidget(_closeButton);
                    addWidget(hLayout);
                    addSeparator();
                    addWidget(scrollWidget, UI::Layout::RowStretch::Expand);
                }

                SettingsWidget()
                {}

            public:
                static std::shared_ptr<SettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                    out->_init(context);
                    return out;
                }
                
                void setCloseCallback(const std::function<void(void)> & value)
                {
                    _closeButton->setClickedCallback(value);
                }

            protected:
                void _localeChangedEvent(Event::LocaleChanged &) override
                {
                    _titleLabel->setText(_getText(DJV_TEXT("djv::ViewLib", "Settings")));
                }

            private:
                std::shared_ptr<UI::Label> _titleLabel;
                std::shared_ptr<UI::Button::Tool> _closeButton;
            };
            
            class DisplaySettingsWidget : public UI::Layout::GroupBox
            {
                DJV_NON_COPYABLE(DisplaySettingsWidget);

            protected:
                void _init(Context * context)
                {
                    GroupBox::_init(context);

                    _buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);

                    _layout = UI::Layout::Flow::create(context);
                    addWidget(_layout);

                    auto weak = std::weak_ptr<DisplaySettingsWidget>(std::dynamic_pointer_cast<DisplaySettingsWidget>(shared_from_this()));
                    _buttonGroup->setRadioCallback(
                        [weak, context](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                            {
                                const auto i = widget->_indexToDPI.find(value);
                                if (i != widget->_indexToDPI.end())
                                {
                                    uiSystem->getStyleSettings()->setDPI(i->second);
                                }
                            }
                        }
                    });

                    if (auto avSystem = context->getSystemT<AV::AVSystem>().lock())
                    {
                        _dpiListObserver = ListObserver<int>::create(
                            avSystem->observeDPIList(),
                            [weak, context](const std::vector<int> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_buttons.clear();
                                widget->_buttonGroup->clearButtons();
                                widget->_layout->clearWidgets();
                                widget->_indexToDPI.clear();
                                widget->_dpiToIndex.clear();
                                int j = 0;
                                for (const auto & i : value)
                                {
                                    auto customStyle = UI::Style::Style::create(context);
                                    customStyle->setDPI(i);
                                    widget->_customStyles.push_back(customStyle);
                                    auto button = UI::Button::Push::create(context);
                                    button->setStyle(customStyle);
                                    widget->_buttons.push_back(button);
                                    widget->_buttonGroup->addButton(button);
                                    widget->_layout->addWidget(button);
                                    widget->_indexToDPI[j] = i;
                                    widget->_dpiToIndex[i] = j;
                                    ++j;
                                }
                                widget->_updateButtonText();
                            }
                        });
                    }

                    if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                    {
                        _dpiObserver = ValueObserver<int>::create(
                            uiSystem->getStyleSettings()->observeDPI(),
                            [weak](int value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_dpiToIndex.find(value);
                                if (i != widget->_dpiToIndex.end())
                                {
                                    widget->_buttonGroup->setChecked(i->second);
                                }
                            }
                        });
                    }
                }

                DisplaySettingsWidget()
                {}

            public:
                static std::shared_ptr<DisplaySettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<DisplaySettingsWidget>(new DisplaySettingsWidget);
                    out->_init(context);
                    return out;
                }

            protected:
                void _localeChangedEvent(Event::LocaleChanged &) override
                {
                    setText(_getText(DJV_TEXT("djv::ViewLib", "Display")));
                    _updateButtonText();
                }

                void _styleChangedEvent(Event::StyleChanged &) override
                {
                    if (auto style = _getStyle().lock())
                    {
                        for (auto i : _customStyles)
                        {
                            i->setPalette(style->getPalette());
                            i->setMetrics(style->getMetrics());
                            i->setFont(style->getFont());
                        }
                    }
                }

            private:
                void _updateButtonText()
                {
                    int j = 0;
                    for (auto i : _buttons)
                    {
                        const auto k = _indexToDPI[j];
                        std::stringstream ss;
                        ss << k << " " << _getText(DJV_TEXT("djv::ViewLib", "DPI"));
                        i->setText(ss.str());
                        ++j;
                    }
                }

                std::vector<std::shared_ptr<UI::Button::Push> > _buttons;
                std::vector<std::shared_ptr<UI::Style::Style> > _customStyles;
                std::shared_ptr<UI::Button::Group> _buttonGroup;
                std::shared_ptr<UI::Layout::Flow> _layout;
                std::map<int, int> _indexToDPI;
                std::map<int, int> _dpiToIndex;
                std::shared_ptr<ListObserver<int> > _dpiListObserver;
                std::shared_ptr<ValueObserver<int> > _dpiObserver;
            };

            class LanguageSettingsWidget : public UI::Layout::GroupBox
            {
                DJV_NON_COPYABLE(LanguageSettingsWidget);

            protected:
                void _init(Context * context)
                {
                    GroupBox::_init(context);

                    _localeComboBox = UI::ComboBox::create(context);

                    _layout = UI::Layout::Form::create(context);
                    _labelIDs["Locale"] = _layout->addWidget(std::string(), _localeComboBox);
                    addWidget(_layout);

                    _widgetUpdate();

                    auto weak = std::weak_ptr<LanguageSettingsWidget>(std::dynamic_pointer_cast<LanguageSettingsWidget>(shared_from_this()));
                    _localeComboBox->setCallback(
                        [weak, context](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto textSystem = context->getSystemT<TextSystem>().lock())
                            {
                                const auto i = widget->_indexToLocale.find(value);
                                if (i != widget->_indexToLocale.end())
                                {
                                    textSystem->setCurrentLocale(i->second);
                                }
                            }
                        }
                    });

                    if (auto textSystem = context->getSystemT<TextSystem>().lock())
                    {
                        _localeObserver = ValueObserver<std::string>::create(
                            textSystem->observeCurrentLocale(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_localeToIndex.find(value);
                                if (i != widget->_localeToIndex.end())
                                {
                                    widget->_localeComboBox->setCurrentItem(static_cast<int>(i->second));
                                }
                            }
                        });
                    }
                }

                LanguageSettingsWidget()
                {}

            public:
                static std::shared_ptr<LanguageSettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<LanguageSettingsWidget>(new LanguageSettingsWidget);
                    out->_init(context);
                    return out;
                }

            protected:
                void _localeChangedEvent(Event::LocaleChanged &) override
                {
                    setText(_getText(DJV_TEXT("djv::ViewLib", "Language")));
                    _widgetUpdate();
                    _layout->setText(_labelIDs["Locale"], _getText(DJV_TEXT("djv::ViewLib", "Locale")));
                }

            private:
                void _widgetUpdate()
                {
                    const int currentItem = _localeComboBox->getCurrentItem();
                    _localeComboBox->clearItems();
                    _indexToLocale.clear();
                    _localeToIndex.clear();
                    auto context = getContext();
                    if (auto textSystem = context->getSystemT<TextSystem>().lock())
                    {
                        size_t j = 0;
                        for (const auto & i : textSystem->getLocales())
                        {
                            _localeComboBox->addItem(_getText(i));
                            _indexToLocale[j] = i;
                            _localeToIndex[i] = j;
                            ++j;
                        }
                    }
                    _localeComboBox->setCurrentItem(currentItem);
                }

                std::shared_ptr<UI::ComboBox> _localeComboBox;
                std::map<size_t, std::string> _indexToLocale;
                std::map<std::string, size_t> _localeToIndex;
                std::map<std::string, size_t> _labelIDs;
                std::shared_ptr<UI::Layout::Form> _layout;
                std::shared_ptr<ValueObserver<std::string> > _localeObserver;
            };

            class ColorPaletteSettingsWidget : public UI::Layout::GroupBox
            {
                DJV_NON_COPYABLE(ColorPaletteSettingsWidget);

            protected:
                void _init(Context * context)
                {
                    GroupBox::_init(context);

                    _paletteComboBox = UI::ComboBox::create(context);

                    _layout = UI::Layout::Form::create(context);
                    _labelIDs["ColorPalette"] = _layout->addWidget(std::string(), _paletteComboBox);
                    addWidget(_layout);

                    auto weak = std::weak_ptr<ColorPaletteSettingsWidget>(std::dynamic_pointer_cast<ColorPaletteSettingsWidget>(shared_from_this()));
                    _paletteComboBox->setCallback(
                        [weak, context](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                            {
                                const auto i = widget->_indexToPalette.find(value);
                                if (i != widget->_indexToPalette.end())
                                {
                                    uiSystem->getStyleSettings()->setCurrentPalette(i->second);
                                }
                            }
                        }
                    });

                    if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                    {
                        _palettesObserver = MapObserver<std::string, UI::Style::Palette>::create(
                            uiSystem->getStyleSettings()->observePalettes(),
                            [weak](const std::map<std::string, UI::Style::Palette > & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_palettes = value;
                                widget->_paletteUpdate();
                            }
                        });
                        _currentPaletteObserver = ValueObserver<std::string>::create(
                            uiSystem->getStyleSettings()->observeCurrentPaletteName(),
                            [weak](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_paletteToIndex.find(value);
                                if (i != widget->_paletteToIndex.end())
                                {
                                    widget->_paletteComboBox->setCurrentItem(static_cast<int>(i->second));
                                }
                            }
                        });
                    }
                }

                ColorPaletteSettingsWidget()
                {}

            public:
                static std::shared_ptr<ColorPaletteSettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<ColorPaletteSettingsWidget>(new ColorPaletteSettingsWidget);
                    out->_init(context);
                    return out;
                }

            protected:
                void _localeChangedEvent(Event::LocaleChanged &) override
                {
                    setText(_getText(DJV_TEXT("djv::ViewLib", "Color Palette")));
                    _paletteUpdate();
                    _layout->setText(_labelIDs["ColorPalette"], _getText(DJV_TEXT("djv::ViewLib", "Color palette")));
                }

            private:
                void _paletteUpdate()
                {
                    const int currentItem = _paletteComboBox->getCurrentItem();
                    _paletteComboBox->clearItems();
                    _indexToPalette.clear();
                    _paletteToIndex.clear();
                    size_t j = 0;
                    for (const auto & i : _palettes)
                    {
                        _paletteComboBox->addItem(_getText(i.first));
                        _indexToPalette[j] = i.first;
                        _paletteToIndex[i.first] = j;
                        ++j;
                    }
                    _paletteComboBox->setCurrentItem(currentItem);
                }

                std::map<std::string, UI::Style::Palette > _palettes;
                std::shared_ptr<UI::ComboBox> _paletteComboBox;
                std::map<size_t, std::string> _indexToPalette;
                std::map<std::string, size_t> _paletteToIndex;
                std::map<std::string, size_t> _labelIDs;
                std::shared_ptr<UI::Layout::Form> _layout;
                std::shared_ptr<MapObserver<std::string, UI::Style::Palette> > _palettesObserver;
                std::shared_ptr<ValueObserver<std::string> > _currentPaletteObserver;
            };

        } // namespace
        
        struct SettingsSystem::Private
        {
            std::shared_ptr<SettingsWidget> settingsWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void SettingsSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewLib::SettingsSystem", context);
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {}

        std::shared_ptr<SettingsSystem> SettingsSystem::create(Context * context)
        {
            auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
            out->_init(context);
            return out;
        }

        void SettingsSystem::showSettingsDialog()
        {
            auto context = getContext();
            if (!_p->settingsWidget)
            {
                _p->settingsWidget = SettingsWidget::create(context);

                auto border = UI::Layout::Border::create(context);
                border->addWidget(_p->settingsWidget);

                _p->overlay = UI::Layout::Overlay::create(context);
                _p->overlay->setBackgroundRole(UI::Style::ColorRole::Overlay);
                _p->overlay->setMargin(UI::Style::MetricsRole::MarginLarge);
                _p->overlay->addWidget(border);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    window->addWidget(_p->overlay);
                    _p->overlay->show();

                    auto weak = std::weak_ptr<SettingsSystem>(std::dynamic_pointer_cast<SettingsSystem>(shared_from_this()));
                    _p->settingsWidget->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                    _p->overlay->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->overlay);
                        }
                    });
                }
            }
        }
        
        std::map<std::string, std::shared_ptr<UI::Action> > SettingsSystem::getActions()
        {
            return _p->actions;
        }

        std::vector<NewSettingsWidget> SettingsSystem::createSettingsWidgets()
        {
            return
            {
                { DisplaySettingsWidget::create(getContext()), "HA" },
                { LanguageSettingsWidget::create(getContext()), "HB" },
                { ColorPaletteSettingsWidget::create(getContext()), "HC" }
            };
        }
        
    } // namespace ViewLib
} // namespace djv

