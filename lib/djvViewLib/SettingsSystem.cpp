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
#include <djvUI/ComboBox.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Icon.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>

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
                    
                    auto titleIcon = UI::Icon::create(context);
                    titleIcon->setIcon("djvIconSettings");
                    titleIcon->setMargin(UI::Layout::Margin(
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::None,
                        UI::Style::MetricsRole::MarginSmall,
                        UI::Style::MetricsRole::MarginSmall));
                    
                    auto titleLabel = UI::Label::create(context);
                    titleLabel->setText(DJV_TEXT("Settings"));
                    titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontLarge);
                    titleLabel->setTextHAlign(UI::TextHAlign::Left);
                    titleLabel->setMargin(UI::Layout::Margin(
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
                            if (auto widget = system->createSettingsWidget())
                            {
                                settingsWidgets[system->getSettingsSortKey()] = widget;
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
                    hLayout->addWidget(titleLabel, UI::Layout::RowStretch::Expand);
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
                
            private:
                std::shared_ptr<UI::Button::Tool> _closeButton;
            };
            
            class GeneralSettingsWidget : public UI::Layout::GroupBox
            {
                DJV_NON_COPYABLE(GeneralSettingsWidget);

            protected:
                void _init(Context * context)
                {
                    GroupBox::_init(context);
                    
                    setText(DJV_TEXT("General"));

                    auto paletteComboBox = UI::ComboBox::create(context);
                    auto dpiComboBox = UI::ComboBox::create(context);

                    auto formLayout = UI::Layout::Form::create(context);
                    formLayout->addWidget(DJV_TEXT("Color palette"), paletteComboBox);
                    formLayout->addWidget(DJV_TEXT("DPI"), dpiComboBox);
                    addWidget(formLayout);

                    auto weak = std::weak_ptr<GeneralSettingsWidget>(std::dynamic_pointer_cast<GeneralSettingsWidget>(shared_from_this()));
                    paletteComboBox->setCallback(
                        [weak, context](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto style = context->getSystemT<UI::Style::Style>().lock())
                            {
                                const auto i = widget->_indexToPalette.find(value);
                                if (i != widget->_indexToPalette.end())
                                {
                                    style->getSettings()->setCurrentPalette(i->second);
                                }
                            }
                        }
                    });
                    dpiComboBox->setCallback(
                        [weak, context](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto style = context->getSystemT<UI::Style::Style>().lock())
                            {
                                const auto i = widget->_indexToDPI.find(value);
                                if (i != widget->_indexToDPI.end())
                                {
                                    style->getSettings()->setDPI(i->second);
                                }
                            }
                        }
                    });

                    if (auto avSystem = context->getSystemT<AV::AVSystem>().lock())
                    {
                        _dpiListObserver = ListObserver<int>::create(
                            avSystem->observeDPIList(),
                            [weak, dpiComboBox](const std::vector<int> & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_indexToDPI.clear();
                                widget->_dpiToIndex.clear();
                                dpiComboBox->clearItems();
                                size_t j = 0;
                                for (const auto & i : value)
                                {
                                    widget->_indexToDPI[j] = i;
                                    widget->_dpiToIndex[i] = j;
                                    std::stringstream ss;
                                    ss << i;
                                    dpiComboBox->addItem(ss.str());
                                    ++j;
                                }
                            }
                        });
                    }

                    if (auto style = _getStyle().lock())
                    {
                        _palettesObserver = MapObserver<std::string, UI::Style::Palette>::create(
                            style->getSettings()->observePalettes(),
                            [weak, paletteComboBox](const std::map<std::string, UI::Style::Palette > & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_indexToPalette.clear();
                                widget->_paletteToIndex.clear();
                                paletteComboBox->clearItems();
                                size_t j = 0;
                                for (const auto & i : value)
                                {
                                    widget->_indexToPalette[j] = i.first;
                                    widget->_paletteToIndex[i.first] = j;
                                    paletteComboBox->addItem(i.first);
                                    ++j;
                                }
                            }
                        });
                        _currentPaletteObserver = ValueObserver<std::string>::create(
                            style->getSettings()->observeCurrentPaletteName(),
                            [weak, paletteComboBox](const std::string & value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_paletteToIndex.find(value);
                                if (i != widget->_paletteToIndex.end())
                                {
                                    paletteComboBox->setCurrentItem(static_cast<int>(i->second));
                                }
                            }
                        });
                        _dpiObserver = ValueObserver<int>::create(
                            style->getSettings()->observeDPI(),
                            [weak, dpiComboBox](int value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = widget->_dpiToIndex.find(value);
                                if (i != widget->_dpiToIndex.end())
                                {
                                    dpiComboBox->setCurrentItem(static_cast<int>(i->second));
                                }
                            }
                        });
                    }
                }

                GeneralSettingsWidget()
                {}

            public:
                static std::shared_ptr<GeneralSettingsWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<GeneralSettingsWidget>(new GeneralSettingsWidget);
                    out->_init(context);
                    return out;
                }

            private:
                std::map<size_t, std::string> _indexToPalette;
                std::map<std::string, size_t> _paletteToIndex;
                std::map<size_t, int> _indexToDPI;
                std::map<int, size_t> _dpiToIndex;
                std::shared_ptr<MapObserver<std::string, UI::Style::Palette> > _palettesObserver;
                std::shared_ptr<ValueObserver<std::string> > _currentPaletteObserver;
                std::shared_ptr<ListObserver<int> > _dpiListObserver;
                std::shared_ptr<ValueObserver<int> > _dpiObserver;
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

            DJV_PRIVATE_PTR();

            auto weak = std::weak_ptr<SettingsSystem>(std::dynamic_pointer_cast<SettingsSystem>(shared_from_this()));
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

        std::shared_ptr<UI::Widget> SettingsSystem::createSettingsWidget()
        {
            return GeneralSettingsWidget::create(getContext());
        }
        
        std::string SettingsSystem::getSettingsSortKey() const
        {
            return "9";
        }

    } // namespace ViewLib
} // namespace djv

