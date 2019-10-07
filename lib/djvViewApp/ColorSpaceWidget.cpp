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

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/EventSystem.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ListButton.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TabWidget.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            std::vector<AV::OCIO::Config> configs;
            AV::OCIO::Config config;
            int currentConfig = -1;
            bool editConfig = false;
            std::vector<std::string> colorSpaces;
            bool editColorProfiles = false;
            std::vector<AV::OCIO::Display> displays;
            std::vector<std::string> views;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");

            std::shared_ptr<UI::ButtonGroup> configButtonGroup;
            std::shared_ptr<UI::ButtonGroup> editConfigButtonGroup;
            std::shared_ptr<UI::ToolButton> addConfigButton;
            std::shared_ptr<UI::ToolButton> editConfigButton;
            std::shared_ptr<UI::VerticalLayout> configItemLayout;
            std::shared_ptr<UI::ScrollWidget> configScrollWidget;
            std::shared_ptr<UI::ButtonGroup> editColorProfilesButtonGroup;
            std::shared_ptr<UI::FormLayout> colorProfilesItemLayout;
            std::shared_ptr<UI::VerticalLayout> addColorProfileLayout;
            std::shared_ptr<UI::PopupWidget> addColorProfilePopupWidget;
            std::shared_ptr<UI::ToolButton> editColorProfilesButton;
            std::shared_ptr<UI::ScrollWidget> colorProfilesScrollWidget;
            std::shared_ptr<UI::ComboBox> displayComboBox;
            std::shared_ptr<UI::ComboBox> viewComboBox;
            std::shared_ptr<UI::FormLayout> displayLayout;
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::shared_ptr<UI::Widget>, size_t> widgetToTab;
            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;
            std::shared_ptr<UI::Window> fileBrowserWindow;
            std::map<std::shared_ptr<UI::Widget>, int> textFocusWidgets;

            std::shared_ptr<ListObserver<AV::OCIO::Config> > configsObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > configObserver;
            std::shared_ptr<ValueObserver<int> > currentConfigObserver;
            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<AV::OCIO::Display> > displaysObserver;
            std::shared_ptr<ListObserver<std::string> > viewsObserver;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.configButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            p.editConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.addConfigButton = UI::ToolButton::create(context);
            p.addConfigButton->setIcon("djvIconAddSmall");
            p.editConfigButton = UI::ToolButton::create(context);
            p.editConfigButton->setButtonType(UI::ButtonType::Toggle);
            p.editConfigButton->setIcon("djvIconEditSmall");

            p.editColorProfilesButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.addColorProfileLayout = UI::VerticalLayout::create(context);
            p.addColorProfileLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.addColorProfilePopupWidget = UI::PopupWidget::create(context);
            p.addColorProfilePopupWidget->setIcon("djvIconAddSmall");
            p.addColorProfilePopupWidget->addChild(p.addColorProfileLayout);
            p.editColorProfilesButton = UI::ToolButton::create(context);
            p.editColorProfilesButton->setButtonType(UI::ButtonType::Toggle);
            p.editColorProfilesButton->setIcon("djvIconEditSmall");

            p.displayComboBox = UI::ComboBox::create(context);
            p.displayComboBox->setHAlign(UI::HAlign::Fill);
            p.viewComboBox = UI::ComboBox::create(context);
            p.viewComboBox->setHAlign(UI::HAlign::Fill);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            addChild(p.tabWidget);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.configItemLayout = UI::VerticalLayout::create(context);
            p.configItemLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.configItemLayout);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.addConfigButton);
            hLayout->addChild(p.editConfigButton);
            vLayout->addChild(hLayout);
            p.configScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.configScrollWidget->setBorder(false);
            p.configScrollWidget->setShadowOverlay({ UI::Side::Top });
            p.configScrollWidget->addChild(vLayout);
            p.widgetToTab[p.configScrollWidget] = p.tabWidget->addTab(std::string(), p.configScrollWidget);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.colorProfilesItemLayout = UI::FormLayout::create(context);
            p.colorProfilesItemLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            vLayout->addChild(p.colorProfilesItemLayout);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.addColorProfilePopupWidget);
            hLayout->addChild(p.editColorProfilesButton);
            vLayout->addChild(hLayout);
            p.colorProfilesScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.colorProfilesScrollWidget->setBorder(false);
            p.colorProfilesScrollWidget->setShadowOverlay({ UI::Side::Top });
            p.colorProfilesScrollWidget->addChild(vLayout);
            p.widgetToTab[p.colorProfilesScrollWidget] = p.tabWidget->addTab(std::string(), p.colorProfilesScrollWidget);

            p.displayLayout = UI::FormLayout::create(context);
            p.displayLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.displayLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.displayLayout->setShadowOverlay({ UI::Side::Top });
            p.displayLayout->addChild(p.displayComboBox);
            p.displayLayout->addChild(p.viewComboBox);
            p.widgetToTab[p.displayLayout] = p.tabWidget->addTab(std::string(), p.displayLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.configButtonGroup->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value >= 0 && value < widget->_p->configs.size())
                            {
                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                ocioSystem->setCurrentConfig(value);
                            }
                        }
                    }
                });

            p.addConfigButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->fileBrowserWindow)
                            {
                                widget->_p->fileBrowserWindow->close();
                                widget->_p->fileBrowserWindow.reset();
                            }
                            widget->_p->fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
                            widget->_p->fileBrowserDialog->setFileExtensions({ ".ocio" });
                            widget->_p->fileBrowserDialog->setPath(widget->_p->fileBrowserPath);
                            widget->_p->fileBrowserDialog->setCallback(
                                [weak, contextWeak](const Core::FileSystem::FileInfo& value)
                                {
                                    if (auto context = contextWeak.lock())
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                            widget->_p->fileBrowserDialog.reset();
                                            widget->_p->fileBrowserWindow->close();
                                            widget->_p->fileBrowserWindow.reset();
                                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                            AV::OCIO::Config config;
                                            config.fileName = std::string(value.getPath());
                                            config.name = AV::OCIO::Config::getNameFromFileName(config.fileName);
                                            ocioSystem->addConfig(config);
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                        widget->_p->fileBrowserDialog.reset();
                                        widget->_p->fileBrowserWindow->close();
                                        widget->_p->fileBrowserWindow.reset();
                                    }
                                });
                            widget->_p->fileBrowserWindow = UI::Window::create(context);
                            widget->_p->fileBrowserWindow->setBackgroundRole(UI::ColorRole::None);
                            widget->_p->fileBrowserWindow->addChild(widget->_p->fileBrowserDialog);
                            widget->_p->fileBrowserWindow->show();
                        }
                    }
                });

            p.editConfigButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->editConfig = value;
                        widget->_widgetUpdate();
                    }
                });

            p.editColorProfilesButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->editColorProfiles = value;
                        widget->_widgetUpdate();
                    }
                });

            p.displayComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (value >= 0 && value < widget->_p->displays.size())
                                {
                                    auto config = widget->_p->config;
                                    config.display = widget->_p->displays[value].name;
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setConfig(config);
                                }
                            }
                        }
                    }
                });

            p.viewComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (value >= 0 && value < widget->_p->views.size())
                                {
                                    auto config = widget->_p->config;
                                    config.view = widget->_p->views[value];
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setConfig(config);
                                }
                            }
                        }
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configsObserver = ListObserver<AV::OCIO::Config>::create(
                ocioSystem->observeConfigs(),
                [weak](const std::vector<AV::OCIO::Config>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configs = value;
                        widget->_widgetUpdate();
                    }
                });

            p.configObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->config = value;
                        widget->_widgetUpdate();
                    }
                });

            p.currentConfigObserver = ValueObserver<int>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentConfig = value;
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.colorSpacesObserver = ListObserver<std::string>::create(
                ocioSystem->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });

            p.displaysObserver = ListObserver<AV::OCIO::Display>::create(
                ocioSystem->observeDisplays(),
                [weak](const std::vector<AV::OCIO::Display>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displays = value;
                        widget->_widgetUpdate();
                    }
                });

            p.viewsObserver = ListObserver<std::string>::create(
                ocioSystem->observeViews(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->views = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserWindow)
            {
                p.fileBrowserWindow->close();
            }
        }

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        int ColorSpaceWidget::getCurrentTab() const
        {
            return _p->tabWidget->getCurrentTab();
        }

        void ColorSpaceWidget::setCurrentTab(int value)
        {
            _p->tabWidget->setCurrentTab(value);
        }

        void ColorSpaceWidget::_localeEvent(Event::Locale & event)
        {
            MDIWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Color Space")));
            p.tabWidget->setText(p.widgetToTab[p.configScrollWidget], DJV_TEXT("Config"));
            p.tabWidget->setText(p.widgetToTab[p.colorProfilesScrollWidget], DJV_TEXT("Image"));
            p.tabWidget->setText(p.widgetToTab[p.displayLayout], DJV_TEXT("Display"));
            p.addConfigButton->setTooltip(_getText(DJV_TEXT("Color space add config tooltip")));
            p.editConfigButton->setTooltip(_getText(DJV_TEXT("Color space edit configs tooltip")));
            p.addColorProfilePopupWidget->setTooltip(_getText(DJV_TEXT("Color space add color profile tooltip")));
            p.editColorProfilesButton->setTooltip(_getText(DJV_TEXT("Color space edit color profiles tooltip")));
            p.displayLayout->setText(p.displayComboBox, _getText(DJV_TEXT("Name")) + ":");
            p.displayLayout->setText(p.viewComboBox, _getText(DJV_TEXT("View")) + ":");
            _widgetUpdate();
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                int restoreTextFocusID = -1;
                auto eventSystem = context->getSystemT<UI::EventSystem>();
                if (auto textFocus = std::dynamic_pointer_cast<UI::Widget>(eventSystem->getTextFocus().lock()))
                {
                    const auto i = p.textFocusWidgets.find(textFocus);
                    if (i != p.textFocusWidgets.end())
                    {
                        restoreTextFocusID = i->second;
                    }
                }
                p.textFocusWidgets.clear();
                int id = 0;

                p.configButtonGroup->clearButtons();
                p.editConfigButtonGroup->clearButtons();
                p.configItemLayout->clearChildren();
                auto contextWeak = std::weak_ptr<Context>(context);
                int j = 0;
                for (const auto& i : p.configs)
                {
                    auto button = UI::CheckBox::create(context);
                    button->setText(i.name);
                    p.configButtonGroup->addButton(button);
                    p.textFocusWidgets[button->getFocusWidget()] = id++;

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconCloseSmall");
                    deleteButton->setVisible(p.editConfig);
                    deleteButton->setInsideMargin(UI::Layout::Margin(UI::MetricsRole::None));
                    deleteButton->setVAlign(UI::VAlign::Fill);
                    p.textFocusWidgets[deleteButton->getFocusWidget()] = id++;

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.configItemLayout->addChild(hLayout);

                    deleteButton->setClickedCallback(
                        [j, contextWeak]
                        {
                            if (auto context = contextWeak.lock())
                            {
                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                ocioSystem->removeConfig(j);
                            }
                        });

                    ++j;
                }
                p.configButtonGroup->setChecked(p.currentConfig);
                p.editConfigButton->setEnabled(p.configs.size() > 0);

                p.editColorProfilesButtonGroup->clearButtons();
                p.colorProfilesItemLayout->clearChildren();
                std::set<std::string> usedPluginNames;
                auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
                for (const auto& i : p.config.colorSpaces)
                {
                    usedPluginNames.insert(i.first);

                    auto comboBox = UI::ComboBox::create(context);
                    comboBox->setHAlign(UI::HAlign::Fill);
                    for (const auto& j : p.colorSpaces)
                    {
                        std::string s = j;
                        if (s.empty())
                        {
                            s = _getText(DJV_TEXT("None"));
                        }
                        comboBox->addItem(s);
                    }
                    int index = -1;
                    int k = 0;
                    for (const auto& j : p.colorSpaces)
                    {
                        if (i.second == j)
                        {
                            index = k;
                            break;
                        }
                        ++k;
                    }
                    comboBox->setCurrentItem(index);
                    p.textFocusWidgets[comboBox->getFocusWidget()] = id++;

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconCloseSmall");
                    deleteButton->setVisible(p.editColorProfiles);
                    deleteButton->setInsideMargin(UI::Layout::Margin(UI::MetricsRole::None));
                    deleteButton->setVAlign(UI::VAlign::Fill);
                    p.textFocusWidgets[deleteButton->getFocusWidget()] = id++;

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    hLayout->addChild(comboBox);
                    hLayout->setStretch(comboBox, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.colorProfilesItemLayout->addChild(hLayout);
                    std::string s = i.first;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("Default"));
                    }
                    p.colorProfilesItemLayout->setText(hLayout, s + ":");

                    std::string pluginName = i.first;
                    comboBox->setCallback(
                        [pluginName, weak, contextWeak](int value)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto widget = weak.lock())
                                {
                                    if (value >= 0 && value < widget->_p->colorSpaces.size())
                                    {
                                        AV::OCIO::Config config = widget->_p->config;
                                        config.colorSpaces[pluginName] = widget->_p->colorSpaces[value];
                                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                        ocioSystem->setConfig(config);
                                    }
                                }
                            }
                        });
                    deleteButton->setClickedCallback(
                        [pluginName, weak, contextWeak]
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto widget = weak.lock())
                                {
                                    AV::OCIO::Config config = widget->_p->config;
                                    auto i = config.colorSpaces.find(pluginName);
                                    if (i != config.colorSpaces.end())
                                    {
                                        config.colorSpaces.erase(i);
                                    }
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setConfig(config);
                                }
                            }
                        });
                }

                p.addColorProfileLayout->clearChildren();
                auto io = context->getSystemT<AV::IO::System>();
                auto pluginNames = io->getPluginNames();
                pluginNames.insert(pluginNames.begin(), std::string());
                for (const auto& i : pluginNames)
                {
                    const auto j = usedPluginNames.find(i);
                    if (j == usedPluginNames.end())
                    {
                        auto button = UI::ListButton::create(context);
                        std::string s = i;
                        if (!s.empty())
                        {
                            s = _getText(i);
                        }
                        else
                        {
                            s = _getText("Default");
                        }
                        button->setText(s);
                        button->setInsideMargin(UI::Layout::Margin(UI::MetricsRole::Margin));
                        p.addColorProfileLayout->addChild(button);
                        std::string pluginName = i;
                        button->setClickedCallback(
                            [pluginName, weak, contextWeak]
                            {
                                if (auto context = contextWeak.lock())
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->addColorProfilePopupWidget->close();
                                        widget->_p->config.colorSpaces[pluginName] = std::string();
                                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                        ocioSystem->setConfig(widget->_p->config);
                                    }
                                }
                            });
                        p.textFocusWidgets[button->getFocusWidget()] = id++;
                    }
                }

                p.addColorProfilePopupWidget->setEnabled(
                    p.configs.size() > 0 &&
                    usedPluginNames.size() < pluginNames.size());
                p.editColorProfilesButton->setEnabled(
                    p.configs.size() > 0 &&
                    p.config.colorSpaces.size() > 0);

                p.displayComboBox->clearItems();
                for (const auto& i : p.displays)
                {
                    std::string s = i.name;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    p.displayComboBox->addItem(s);
                }
                int index = -1;
                if (p.currentConfig >= 0 && p.currentConfig < p.configs.size())
                {
                    const auto& config = p.configs[p.currentConfig];
                    int j = 0;
                    for (const auto& i : p.displays)
                    {
                        if (config.display == i.name)
                        {
                            index = j;
                            break;
                        }
                        ++j;
                    }
                }
                p.displayComboBox->setCurrentItem(index);
                p.textFocusWidgets[p.displayComboBox->getFocusWidget()] = id++;

                p.viewComboBox->setEnabled(p.views.size() > 0);
                p.viewComboBox->clearItems();
                for (const auto& i : p.views)
                {
                    std::string s = i;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    p.viewComboBox->addItem(s);
                }
                index = -1;
                j = 0;
                for (const auto& i : p.views)
                {
                    if (p.config.view == i)
                    {
                        index = j;
                        break;
                    }
                    ++j;
                }
                p.viewComboBox->setCurrentItem(index);
                p.textFocusWidgets[p.viewComboBox->getFocusWidget()] = id++;

                if (restoreTextFocusID != -1)
                {
                    for (const auto& i : p.textFocusWidgets)
                    {
                        if (i.second == restoreTextFocusID)
                        {
                            i.first->takeTextFocus();
                            break;
                        }
                    }
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

