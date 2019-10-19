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
#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/EventSystem.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
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
        namespace
        {
            class ListWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ListWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                ListWidget()
                {}

            public:
                static std::shared_ptr<ListWidget> create(const std::shared_ptr<Context>&);

                void setItems(const std::vector<std::string>&);

                void setCallback(const std::function<void(int)>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

            private:
                void _widgetUpdate();

                std::vector<std::string> _items;
                std::string _filter;
                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::shared_ptr<UI::VerticalLayout> _buttonLayout;
                std::shared_ptr<UI::SearchBox> _searchBox;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::function<void(int)> _callback;
            };

            void ListWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

                _buttonLayout = UI::VerticalLayout::create(context);
                _buttonLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));

                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(_buttonLayout);

                _searchBox = UI::SearchBox::create(context);
                _searchBox->setBorder(false);

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                _layout->addChild(scrollWidget);
                _layout->addSeparator();
                _layout->addChild(_searchBox);
                addChild(_layout);

                auto weak = std::weak_ptr<ListWidget>(std::dynamic_pointer_cast<ListWidget>(shared_from_this()));
                _buttonGroup->setPushCallback(
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_callback)
                            {
                                widget->_callback(value);
                            }
                        }
                    });

                _searchBox->setFilterCallback(
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_filter = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            std::shared_ptr<ListWidget> ListWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ListWidget>(new ListWidget);
                out->_init(context);
                return out;
            }

            void ListWidget::setItems(const std::vector<std::string>& value)
            {
                if (value == _items)
                    return;
                _items = value;
                _widgetUpdate();
            }

            void ListWidget::setCallback(const std::function<void(int)>& value)
            {
                _callback = value;
            }

            void ListWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ListWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void ListWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    _buttonGroup->clearButtons();
                    _buttonLayout->clearChildren();
                    for (const auto& i : _items)
                    {
                        auto button = UI::ListButton::create(context);
                        button->setText(i);
                        if (!_filter.empty())
                        {
                            button->setVisible(String::match(i, _filter));
                        }
                        _buttonGroup->addButton(button);
                        _buttonLayout->addChild(button);
                    }
                }
            }

            bool hasImageListUpdate(const AV::OCIO::Config& a, const AV::OCIO::Config& b)
            {
                bool out = false;
                if (a.fileName == b.fileName &&
                    a.name == b.name &&
                    a.display == b.display &&
                    a.view == b.view)
                {
                    std::vector<std::string> aKeys;
                    std::vector<std::string> bKeys;
                    for (const auto& j : a.colorSpaces)
                    {
                        aKeys.push_back(j.first);
                    }
                    for (const auto& j : b.colorSpaces)
                    {
                        bKeys.push_back(j.first);
                    }
                    if (aKeys == bKeys)
                    {
                        out = true;
                    }
                }
                return out;
            }

            bool hasImageListUpdate(const std::vector<AV::OCIO::Config>& a, const std::vector<AV::OCIO::Config>& b)
            {
                bool out = false;
                const size_t size = a.size();
                if (size == b.size())
                {
                    size_t i = 0;
                    for (; i < size; ++i)
                    {
                        if (!hasImageListUpdate(a[i], b[i]))
                        {
                            break;
                        }
                    }
                    if (size == i)
                    {
                        out = true;
                    }
                }
                return out;
            }

        } // namespace

        struct ColorSpaceWidget::Private
        {
            std::vector<AV::OCIO::Config> configs;
            AV::OCIO::Config currentConfig;
            int currentIndex = -1;
            bool editConfig = false;
            std::vector<std::string> colorSpaces;
            bool editImage = false;
            std::vector<AV::OCIO::Display> displays;
            std::vector<std::string> views;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");

            std::shared_ptr<UI::ButtonGroup> configButtonGroup;
            std::shared_ptr<UI::ButtonGroup> editConfigButtonGroup;
            std::shared_ptr<UI::ToolButton> addConfigButton;
            std::shared_ptr<UI::ToolButton> editConfigButton;
            std::shared_ptr<UI::FormLayout> configItemLayout;
            std::shared_ptr<UI::VerticalLayout> configLayout;

            std::shared_ptr<UI::ButtonGroup> editImageButtonGroup;
            std::shared_ptr<UI::FormLayout> imageItemLayout;
            std::shared_ptr<UI::VerticalLayout> addImageLayout;
            std::shared_ptr<UI::PopupWidget> addImagePopupWidget;
            std::shared_ptr<UI::ToolButton> editImageButton;
            std::shared_ptr<UI::VerticalLayout> imageLayout;
            std::vector<std::shared_ptr<UI::PopupWidget> > imagePopupWidgets;

            std::shared_ptr<ListWidget> displayListWidget;
            std::shared_ptr<UI::PopupWidget> displayPopupWidget;
            std::shared_ptr<ListWidget> viewListWidget;
            std::shared_ptr<UI::PopupWidget> viewPopupWidget;
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

            p.configButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.editConfigButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.addConfigButton = UI::ToolButton::create(context);
            p.addConfigButton->setIcon("djvIconAddSmall");
            p.editConfigButton = UI::ToolButton::create(context);
            p.editConfigButton->setButtonType(UI::ButtonType::Toggle);
            p.editConfigButton->setIcon("djvIconEditSmall");

            p.editImageButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.addImageLayout = UI::VerticalLayout::create(context);
            p.addImageLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.addImagePopupWidget = UI::PopupWidget::create(context);
            p.addImagePopupWidget->setIcon("djvIconAddSmall");
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.addImageLayout);
            p.addImagePopupWidget->addChild(scrollWidget);
            p.editImageButton = UI::ToolButton::create(context);
            p.editImageButton->setButtonType(UI::ButtonType::Toggle);
            p.editImageButton->setIcon("djvIconEditSmall");

            p.displayListWidget = ListWidget::create(context);
            p.displayPopupWidget = UI::PopupWidget::create(context);
            p.displayPopupWidget->setPopupIcon("djvIconPopupMenu");
            p.displayPopupWidget->addChild(p.displayListWidget);
            p.viewListWidget = ListWidget::create(context);
            p.viewPopupWidget = UI::PopupWidget::create(context);
            p.viewPopupWidget->setPopupIcon("djvIconPopupMenu");
            p.viewPopupWidget->addChild(p.viewListWidget);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            addChild(p.tabWidget);

            p.configLayout = UI::VerticalLayout::create(context);
            p.configLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.configItemLayout = UI::FormLayout::create(context);
            p.configItemLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.configItemLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.configItemLayout);
            p.configLayout->addChild(scrollWidget);
            p.configLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.configLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.addConfigButton);
            hLayout->addChild(p.editConfigButton);
            p.configLayout->addChild(hLayout);
            p.widgetToTab[p.configLayout] = p.tabWidget->addTab(std::string(), p.configLayout);

            p.imageLayout = UI::VerticalLayout::create(context);
            p.imageLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.imageItemLayout = UI::FormLayout::create(context);
            p.imageItemLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.imageItemLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.imageItemLayout);
            p.imageLayout->addChild(scrollWidget);
            p.imageLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.imageLayout->addSeparator();
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout->addExpander();
            hLayout->addChild(p.addImagePopupWidget);
            hLayout->addChild(p.editImageButton);
            p.imageLayout->addChild(hLayout);
            p.widgetToTab[p.imageLayout] = p.tabWidget->addTab(std::string(), p.imageLayout);

            p.displayLayout = UI::FormLayout::create(context);
            p.displayLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            p.displayLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            p.displayLayout->setShadowOverlay({ UI::Side::Top });
            p.displayLayout->addChild(p.displayPopupWidget);
            p.displayLayout->addChild(p.viewPopupWidget);
            p.widgetToTab[p.displayLayout] = p.tabWidget->addTab(std::string(), p.displayLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.configButtonGroup->setExclusiveCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setCurrentIndex(value);
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

            p.editImageButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->editImage = value;
                        widget->_widgetUpdate();
                    }
                });

            p.displayListWidget->setCallback(
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
                                    auto config = widget->_p->currentConfig;
                                    config.display = widget->_p->displays[value].name;
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setCurrentConfig(config);
                                }
                            }
                        }
                    }
                });

            p.viewListWidget->setCallback(
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
                                    auto config = widget->_p->currentConfig;
                                    config.view = widget->_p->views[value];
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setCurrentConfig(config);
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
                        const bool imageListUpdate = hasImageListUpdate(value, widget->_p->configs);
                        widget->_p->configs = value;
                        if (imageListUpdate)
                        {
                            widget->_imageListUpdate();
                        }
                        else
                        {
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.configObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        const bool imageListUpdate = hasImageListUpdate(value, widget->_p->currentConfig);
                        widget->_p->currentConfig = value;
                        if (imageListUpdate)
                        {
                            widget->_imageListUpdate();
                        }
                        else
                        {
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.currentConfigObserver = ValueObserver<int>::create(
                ocioSystem->observeCurrentIndex(),
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentIndex = value;
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

        void ColorSpaceWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            MDIWidget::_textUpdateEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Color Space")));
            p.tabWidget->setText(p.widgetToTab[p.configLayout], DJV_TEXT("Config"));
            p.tabWidget->setText(p.widgetToTab[p.imageLayout], DJV_TEXT("Image"));
            p.tabWidget->setText(p.widgetToTab[p.displayLayout], DJV_TEXT("Display"));
            p.addConfigButton->setTooltip(_getText(DJV_TEXT("Color space add config tooltip")));
            p.editConfigButton->setTooltip(_getText(DJV_TEXT("Color space edit configs tooltip")));
            p.addImagePopupWidget->setTooltip(_getText(DJV_TEXT("Color space add image tooltip")));
            p.editImageButton->setTooltip(_getText(DJV_TEXT("Color space edit images tooltip")));
            p.displayLayout->setText(p.displayPopupWidget, _getText(DJV_TEXT("Name")) + ":");
            p.displayLayout->setText(p.viewPopupWidget, _getText(DJV_TEXT("View")) + ":");
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

                p.imagePopupWidgets.clear();
                p.configButtonGroup->clearButtons();
                p.editConfigButtonGroup->clearButtons();
                p.configItemLayout->clearChildren();
                auto contextWeak = std::weak_ptr<Context>(context);
                int j = 0;
                for (const auto& i : p.configs)
                {
                    auto button = UI::CheckBox::create(context);
                    std::string s = i.name;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    button->setText(s);
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
                p.configButtonGroup->setChecked(p.currentIndex);
                p.editConfigButton->setEnabled(p.configs.size() > 0);

                p.editImageButtonGroup->clearButtons();
                p.imageItemLayout->clearChildren();
                std::set<std::string> usedPluginNames;
                auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
                for (const auto& i : p.currentConfig.colorSpaces)
                {
                    usedPluginNames.insert(i.first);

                    std::vector<std::string> colorSpaces;
                    for (const auto& j : p.colorSpaces)
                    {
                        std::string s = j;
                        if (s.empty())
                        {
                            s = _getText(DJV_TEXT("None"));
                        }
                        colorSpaces.push_back(s);
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
                    auto listWidget = ListWidget::create(context);
                    listWidget->setItems(colorSpaces);

                    auto popupWidget = UI::PopupWidget::create(context);
                    popupWidget->setPopupIcon("djvIconPopupMenu");
                    popupWidget->setText((index >= 0 && index < colorSpaces.size()) ? colorSpaces[index] : std::string());
                    popupWidget->addChild(listWidget);
                    p.imagePopupWidgets.push_back(popupWidget);
                    p.textFocusWidgets[popupWidget->getFocusWidget()] = id++;

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconCloseSmall");
                    deleteButton->setVisible(p.editImage);
                    deleteButton->setInsideMargin(UI::Layout::Margin(UI::MetricsRole::None));
                    deleteButton->setVAlign(UI::VAlign::Fill);
                    p.textFocusWidgets[deleteButton->getFocusWidget()] = id++;

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    hLayout->addChild(popupWidget);
                    hLayout->setStretch(popupWidget, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.imageItemLayout->addChild(hLayout);
                    std::string s = i.first;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("Default"));
                    }
                    p.imageItemLayout->setText(hLayout, s + ":");

                    std::string pluginName = i.first;
                    listWidget->setCallback(
                        [pluginName, weak, contextWeak](int value)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                if (auto widget = weak.lock())
                                {
                                    if (value >= 0 && value < widget->_p->colorSpaces.size())
                                    {
                                        AV::OCIO::Config config = widget->_p->currentConfig;
                                        config.colorSpaces[pluginName] = widget->_p->colorSpaces[value];
                                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                        ocioSystem->setCurrentConfig(config);
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
                                    AV::OCIO::Config config = widget->_p->currentConfig;
                                    auto i = config.colorSpaces.find(pluginName);
                                    if (i != config.colorSpaces.end())
                                    {
                                        config.colorSpaces.erase(i);
                                    }
                                    auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                    ocioSystem->setCurrentConfig(config);
                                }
                            }
                        });
                }

                p.addImageLayout->clearChildren();
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
                        p.addImageLayout->addChild(button);
                        std::string pluginName = i;
                        button->setClickedCallback(
                            [pluginName, weak, contextWeak]
                            {
                                if (auto context = contextWeak.lock())
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->addImagePopupWidget->close();
                                        widget->_p->currentConfig.colorSpaces[pluginName] = std::string();
                                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                        ocioSystem->setCurrentConfig(widget->_p->currentConfig);
                                    }
                                }
                            });
                        p.textFocusWidgets[button->getFocusWidget()] = id++;
                    }
                }

                p.addImagePopupWidget->setEnabled(
                    p.configs.size() > 0 &&
                    usedPluginNames.size() < pluginNames.size());
                p.editImageButton->setEnabled(
                    p.configs.size() > 0 &&
                    p.currentConfig.colorSpaces.size() > 0);

                std::vector<std::string> displays;
                for (const auto& i : p.displays)
                {
                    std::string s = i.name;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    displays.push_back(s);
                }
                int index = -1;
                if (p.currentIndex >= 0 && p.currentIndex < p.configs.size())
                {
                    const auto& config = p.configs[p.currentIndex];
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
                p.displayListWidget->setItems(displays);
                p.displayPopupWidget->setText((index >= 0 && index < displays.size()) ? displays[index] : std::string());
                p.textFocusWidgets[p.displayPopupWidget->getFocusWidget()] = id++;

                p.viewPopupWidget->setEnabled(p.views.size() > 0);
                std::vector<std::string> views;
                for (const auto& i : p.views)
                {
                    std::string s = i;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    views.push_back(s);
                }
                index = -1;
                j = 0;
                for (const auto& i : p.views)
                {
                    if (p.currentConfig.view == i)
                    {
                        index = j;
                        break;
                    }
                    ++j;
                }
                p.viewListWidget->setItems(views);
                p.viewPopupWidget->setText((index >= 0 && index < views.size()) ? views[index] : std::string());
                p.textFocusWidgets[p.viewPopupWidget->getFocusWidget()] = id++;

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

        void ColorSpaceWidget::_imageListUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);

                std::vector<std::string> colorSpaces;
                for (const auto& i : p.colorSpaces)
                {
                    std::string s = i;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("None"));
                    }
                    colorSpaces.push_back(s);
                }

                size_t j = 0;
                for (const auto& i : p.currentConfig.colorSpaces)
                {
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
                    p.imagePopupWidgets[j]->setText(
                        index >= 0 && index < colorSpaces.size() ?
                        colorSpaces[index] :
                        std::string());
                    ++j;
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

