// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                _buttonLayout->setSpacing(UI::MetricsRole::None);

                auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->addChild(_buttonLayout);

                _searchBox = UI::SearchBox::create(context);
                _searchBox->setBorder(false);

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
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
                    for (const auto& j : a.fileColorSpaces)
                    {
                        aKeys.push_back(j.first);
                    }
                    for (const auto& j : b.fileColorSpaces)
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

            std::shared_ptr<ListWidget> displayListWidget;
            std::shared_ptr<UI::PopupWidget> displayPopupWidget;
            std::shared_ptr<ListWidget> viewListWidget;
            std::shared_ptr<UI::PopupWidget> viewPopupWidget;
            std::shared_ptr<UI::FormLayout> displayLayout;

            std::shared_ptr<UI::ButtonGroup> editImageButtonGroup;
            std::shared_ptr<UI::FormLayout> imageItemLayout;
            std::shared_ptr<UI::VerticalLayout> addImageLayout;
            std::shared_ptr<UI::PopupWidget> addImagePopupWidget;
            std::shared_ptr<UI::ToolButton> editImageButton;
            std::shared_ptr<UI::VerticalLayout> imageLayout;
            std::vector<std::shared_ptr<UI::PopupWidget> > imagePopupWidgets;

            std::shared_ptr<UI::TabWidget> tabWidget;
            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;
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

            p.displayListWidget = ListWidget::create(context);
            p.displayPopupWidget = UI::PopupWidget::create(context);
            p.displayPopupWidget->setPopupIcon("djvIconPopupMenu");
            p.displayPopupWidget->addChild(p.displayListWidget);
            p.viewListWidget = ListWidget::create(context);
            p.viewPopupWidget = UI::PopupWidget::create(context);
            p.viewPopupWidget->setPopupIcon("djvIconPopupMenu");
            p.viewPopupWidget->addChild(p.viewListWidget);

            p.editImageButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.addImageLayout = UI::VerticalLayout::create(context);
            p.addImageLayout->setSpacing(UI::MetricsRole::None);
            p.addImagePopupWidget = UI::PopupWidget::create(context);
            p.addImagePopupWidget->setIcon("djvIconAddSmall");
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.addImageLayout);
            p.addImagePopupWidget->addChild(scrollWidget);
            p.editImageButton = UI::ToolButton::create(context);
            p.editImageButton->setButtonType(UI::ButtonType::Toggle);
            p.editImageButton->setIcon("djvIconEditSmall");

            p.configLayout = UI::VerticalLayout::create(context);
            p.configLayout->setSpacing(UI::MetricsRole::None);
            p.configItemLayout = UI::FormLayout::create(context);
            p.configItemLayout->setSpacing(UI::MetricsRole::None);
            p.configItemLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.configItemLayout);
            p.configLayout->addChild(scrollWidget);
            p.configLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.configLayout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addConfigButton);
            hLayout->addChild(p.editConfigButton);
            p.configLayout->addChild(hLayout);

            p.displayLayout = UI::FormLayout::create(context);
            p.displayLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            p.displayLayout->setSpacing(UI::MetricsRole::None);
            p.displayLayout->setShadowOverlay({ UI::Side::Top });
            p.displayLayout->addChild(p.displayPopupWidget);
            p.displayLayout->addChild(p.viewPopupWidget);

            p.imageLayout = UI::VerticalLayout::create(context);
            p.imageLayout->setSpacing(UI::MetricsRole::None);
            p.imageItemLayout = UI::FormLayout::create(context);
            p.imageItemLayout->setSpacing(UI::MetricsRole::None);
            p.imageItemLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.imageItemLayout);
            p.imageLayout->addChild(scrollWidget);
            p.imageLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.imageLayout->addSeparator();
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addImagePopupWidget);
            hLayout->addChild(p.editImageButton);
            p.imageLayout->addChild(hLayout);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setBackgroundRole(UI::ColorRole::Background);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabWidget->addChild(p.configLayout);
            p.tabWidget->addChild(p.displayLayout);
            p.tabWidget->addChild(p.imageLayout);
            addChild(p.tabWidget);

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
                            if (widget->_p->fileBrowserDialog)
                            {
                                widget->_p->fileBrowserDialog->close();
                                widget->_p->fileBrowserDialog.reset();
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
                                            widget->_p->fileBrowserDialog->close();
                                            widget->_p->fileBrowserDialog.reset();
                                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                            ocioSystem->addConfig(value.getPath().get());
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                        widget->_p->fileBrowserDialog->close();
                                        widget->_p->fileBrowserDialog.reset();
                                    }
                                });
                            widget->_p->fileBrowserDialog->show();
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

            p.editImageButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->editImage = value;
                        widget->_widgetUpdate();
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
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
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

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("widget_color_space")));
            p.addConfigButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
            p.editConfigButton->setTooltip(_getText(DJV_TEXT("widget_color_space_edit_configs_tooltip")));
            p.displayLayout->setText(p.displayPopupWidget, _getText(DJV_TEXT("widget_color_space_display_name")) + ":");
            p.displayLayout->setText(p.viewPopupWidget, _getText(DJV_TEXT("widget_color_space_display_view")) + ":");
            p.addImagePopupWidget->setTooltip(_getText(DJV_TEXT("widget_color_space_add_format_tooltip")));
            p.editImageButton->setTooltip(_getText(DJV_TEXT("widget_color_space_edit_format_tooltip")));
            p.tabWidget->setText(p.configLayout, _getText(DJV_TEXT("widget_color_space_config")));
            p.tabWidget->setText(p.displayLayout, _getText(DJV_TEXT("widget_color_space_display")));
            p.tabWidget->setText(p.imageLayout, _getText(DJV_TEXT("widget_color_space_image")));
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
                    button->setText(i.name);
                    button->setTooltip(i.fileName);
                    p.configButtonGroup->addButton(button);
                    p.textFocusWidgets[button->getFocusWidget()] = id++;

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconCloseSmall");
                    deleteButton->setVisible(p.editConfig);
                    deleteButton->setInsideMargin(UI::MetricsRole::None);
                    deleteButton->setVAlign(UI::VAlign::Fill);
                    p.textFocusWidgets[deleteButton->getFocusWidget()] = id++;

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
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

                std::vector<std::string> displays;
                for (const auto& i : p.displays)
                {
                    std::string s = i.name;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("av_ocio_display_none"));
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
                        s = _getText(DJV_TEXT("av_ocio_view_none"));
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

                p.editImageButtonGroup->clearButtons();
                p.imageItemLayout->clearChildren();
                std::set<std::string> usedPluginNames;
                auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
                for (const auto& i : p.currentConfig.fileColorSpaces)
                {
                    usedPluginNames.insert(i.first);

                    std::vector<std::string> colorSpaces;
                    for (const auto& j : p.colorSpaces)
                    {
                        std::string s = j;
                        if (s.empty())
                        {
                            s = _getText(DJV_TEXT("av_ocio_image_none"));
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
                    deleteButton->setInsideMargin(UI::MetricsRole::None);
                    deleteButton->setVAlign(UI::VAlign::Fill);
                    p.textFocusWidgets[deleteButton->getFocusWidget()] = id++;

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(popupWidget);
                    hLayout->setStretch(popupWidget, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.imageItemLayout->addChild(hLayout);
                    std::string s = i.first;
                    if (s.empty())
                    {
                        s = _getText(DJV_TEXT("widget_color_space_display_default"));
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
                                        config.fileColorSpaces[pluginName] = widget->_p->colorSpaces[value];
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
                                    auto i = config.fileColorSpaces.find(pluginName);
                                    if (i != config.fileColorSpaces.end())
                                    {
                                        config.fileColorSpaces.erase(i);
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
                        button->setInsideMargin(UI::MetricsRole::Margin);
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
                                        widget->_p->currentConfig.fileColorSpaces[pluginName] = std::string();
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
                    p.currentConfig.fileColorSpaces.size() > 0);

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
                        s = _getText(DJV_TEXT("av_ocio_image_none"));
                    }
                    colorSpaces.push_back(s);
                }

                size_t j = 0;
                for (const auto& i : p.currentConfig.fileColorSpaces)
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

