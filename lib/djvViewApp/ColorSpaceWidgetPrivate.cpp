// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/EventSystem.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/ListWidget.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IO.h>
#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            std::shared_ptr<UI::Layout::Spacer> createSpacer(const std::shared_ptr<Context>& context)
            {
                auto out = UI::Layout::HorizontalSpacer::create(context);
                out->setSpacerSize(UI::MetricsRole::None);
                out->setSpacerOppositeSize(UI::MetricsRole::Icon);
                out->setMargin(UI::Layout::Margin(
                    UI::MetricsRole::None,
                    UI::MetricsRole::None,
                    UI::MetricsRole::MarginSmall,
                    UI::MetricsRole::MarginSmall));
                return out;
            }

        } // namespace

        struct ColorSpaceConfigsWidget::Private
        {
            AV::OCIO::Config config;
            AV::OCIO::ConfigData data;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");
            bool deleteEnabled = false;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::shared_ptr<UI::ButtonGroup> deleteButtonGroup;
            std::shared_ptr<UI::ToolButton> addButton;
            std::shared_ptr<UI::ToolButton> deleteButton;
            std::shared_ptr<UI::VerticalLayout> buttonLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::Config> > configObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::ConfigData> > dataObserver;
        };

        void ColorSpaceConfigsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceConfigsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.deleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.addButton = UI::ToolButton::create(context);
            p.addButton->setIcon("djvIconAdd");

            p.deleteButton = UI::ToolButton::create(context);
            p.deleteButton->setButtonType(UI::ButtonType::Toggle);
            p.deleteButton->setIcon("djvIconClear");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            p.buttonLayout = UI::VerticalLayout::create(context);
            p.buttonLayout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.buttonLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addButton);
            hLayout->addChild(p.deleteButton);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceConfigsWidget>(std::dynamic_pointer_cast<ColorSpaceConfigsWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.buttonGroup->setExclusiveCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentConfig(static_cast<int>(value));
                    }
                });

            p.deleteButtonGroup->setPushCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->removeConfig(static_cast<int>(value));
                    }
                });

            p.addButton->setClickedCallback(
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

            p.deleteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->deleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak](const AV::OCIO::Config& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->config = value;
                        widget->_widgetUpdate();
                    }
                });

            p.dataObserver = ValueObserver<AV::OCIO::ConfigData>::create(
                ocioSystem->observeConfigData(),
                [weak](const AV::OCIO::ConfigData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->data = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceConfigsWidget::ColorSpaceConfigsWidget() :
            _p(new Private)
        {}

        ColorSpaceConfigsWidget::~ColorSpaceConfigsWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<ColorSpaceConfigsWidget> ColorSpaceConfigsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceConfigsWidget>(new ColorSpaceConfigsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceConfigsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceConfigsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceConfigsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceConfigsWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_config")));
                p.addButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
                p.deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_configs_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorSpaceConfigsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto contextWeak = std::weak_ptr<Context>(context);
                p.buttonGroup->clearButtons();
                p.deleteButtonGroup->clearButtons();
                p.buttonLayout->clearChildren();
                std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
                for (size_t i = 0; i < p.data.names.size(); ++i)
                {
                    auto button = UI::ListButton::create(context);
                    button->setText(p.data.names[i]);
                    button->setTooltip(p.data.fileNames[i]);
                    buttons.push_back(button);
                    
                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconClear");
                    deleteButton->setVisible(p.deleteEnabled);
                    deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_config_tooltip")));
                    p.deleteButtonGroup->addButton(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(createSpacer(context));
                    hLayout->addChild(deleteButton);
                    p.buttonLayout->addChild(hLayout);
                }
                p.buttonGroup->setButtons(buttons, p.data.current);
            }
        }

        struct ColorSpaceDisplaysWidget::Private
        {
            AV::OCIO::DisplayData data;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::DisplayData> > dataObserver;
        };

        void ColorSpaceDisplaysWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceDisplaysWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceDisplaysWidget>(std::dynamic_pointer_cast<ColorSpaceDisplaysWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentDisplay(value);
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.dataObserver = ValueObserver<AV::OCIO::DisplayData>::create(
                ocioSystem->observeDisplayData(),
                [weak](const AV::OCIO::DisplayData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->data = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceDisplaysWidget::ColorSpaceDisplaysWidget() :
            _p(new Private)
        {}

        ColorSpaceDisplaysWidget::~ColorSpaceDisplaysWidget()
        {}

        std::shared_ptr<ColorSpaceDisplaysWidget> ColorSpaceDisplaysWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceDisplaysWidget>(new ColorSpaceDisplaysWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceDisplaysWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceDisplaysWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceDisplaysWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceDisplaysWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_display")));
                _widgetUpdate();
            }
        }

        void ColorSpaceDisplaysWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.data.names.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.data.names[i].empty() ?
                        p.data.names[i] :
                        _getText(DJV_TEXT("av_ocio_display_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items, p.data.current);
            }
        }

        struct ColorSpaceViewsWidget::Private
        {
            AV::OCIO::ViewData data;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::ViewData> > dataObserver;
        };

        void ColorSpaceViewsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceViewsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceViewsWidget>(std::dynamic_pointer_cast<ColorSpaceViewsWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentView(value);
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.dataObserver = ValueObserver<AV::OCIO::ViewData>::create(
                ocioSystem->observeViewData(),
                [weak](const AV::OCIO::ViewData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->data = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceViewsWidget::ColorSpaceViewsWidget() :
            _p(new Private)
        {}

        ColorSpaceViewsWidget::~ColorSpaceViewsWidget()
        {}

        std::shared_ptr<ColorSpaceViewsWidget> ColorSpaceViewsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceViewsWidget>(new ColorSpaceViewsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceViewsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceViewsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceViewsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceViewsWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_view")));
                _widgetUpdate();
            }
        }

        void ColorSpaceViewsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.data.names.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.data.names[i].empty() ?
                        p.data.names[i] :
                        _getText(DJV_TEXT("av_ocio_view_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items, p.data.current);
            }
        }

        struct FileColorSpacesWidget::Private
        {
            std::map<std::string, std::string> fileColorSpaces;
            bool deleteEnabled = false;

            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::shared_ptr<UI::ButtonGroup> deleteButtonGroup;
            std::shared_ptr<UI::PopupButton> addButton;
            std::shared_ptr<UI::ButtonGroup> addButtonGroup;
            std::shared_ptr<UI::ToolButton> deleteButton;
            std::shared_ptr<UI::VerticalLayout> itemLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void(const std::string&)> callback;

            std::shared_ptr<MapObserver<std::string, std::string> > fileColorSpacesObserver;
        };

        void FileColorSpacesWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::FileColorSpacesWidget");

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.deleteButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);

            p.addButton = UI::PopupButton::create(context);
            p.addButton->setIcon("djvIconAdd");
            p.addButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            auto addButtonLayout = UI::VerticalLayout::create(context);
            addButtonLayout->setSpacing(UI::MetricsRole::None);
            auto io = context->getSystemT<AV::IO::System>();
            auto pluginNames = io->getPluginNames();
            for (const auto& j : pluginNames)
            {
                auto button = UI::ListButton::create(context);
                button->setText(j);
                p.addButtonGroup->addButton(button);
                addButtonLayout->addChild(button);
            }
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(addButtonLayout);
            p.addButton->addChild(scrollWidget);

            p.deleteButton = UI::ToolButton::create(context);
            p.deleteButton->setButtonType(UI::ButtonType::Toggle);
            p.deleteButton->setIcon("djvIconClear");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.itemLayout = UI::VerticalLayout::create(context);
            p.itemLayout->setSpacing(UI::MetricsRole::None);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.itemLayout);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addExpander();
            hLayout->addChild(p.addButton);
            hLayout->addChild(p.deleteButton);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto weak = std::weak_ptr<FileColorSpacesWidget>(std::dynamic_pointer_cast<FileColorSpacesWidget>(shared_from_this()));
            p.buttonGroup->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        int index = 0;
                        for (const auto& i : widget->_p->fileColorSpaces)
                        {
                            if (value == index)
                            {
                                if (widget->_p->callback)
                                {
                                    widget->_p->callback(i.first);
                                }
                                break;
                            }
                            ++index;
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.deleteButtonGroup->setPushCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto fileColorSpaces = widget->_p->fileColorSpaces;
                            int index = 0;
                            for (auto i = fileColorSpaces.begin(); i != fileColorSpaces.end(); ++i, ++index)
                            {
                                if (index == value)
                                {
                                    fileColorSpaces.erase(i);
                                    break;
                                }
                            }
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setFileColorSpaces(fileColorSpaces);
                        }
                    }
                });

            std::vector<std::string> pluginNamesList;
            for (const auto& i : pluginNames)
            {
                pluginNamesList.push_back(i);
            }
            p.addButtonGroup->setPushCallback(
                [pluginNamesList, weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->addButton->close();
                            if (value >= 0 && value < static_cast<int>(pluginNamesList.size()))
                            {
                                auto fileColorSpaces = widget->_p->fileColorSpaces;
                                fileColorSpaces[pluginNamesList[value]] = std::string();
                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                ocioSystem->setFileColorSpaces(fileColorSpaces);
                            }
                        }
                    }
                });

            p.deleteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->deleteEnabled = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.fileColorSpacesObserver = MapObserver<std::string, std::string>::create(
                ocioSystem->observeFileColorSpaces(),
                [weak](const std::map<std::string, std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileColorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        FileColorSpacesWidget::FileColorSpacesWidget() :
            _p(new Private)
        {}

        FileColorSpacesWidget::~FileColorSpacesWidget()
        {}

        std::shared_ptr<FileColorSpacesWidget> FileColorSpacesWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FileColorSpacesWidget>(new FileColorSpacesWidget);
            out->_init(context);
            return out;
        }

        void FileColorSpacesWidget::setCallback(const std::function<void(const std::string&)>& value)
        {
            _p->callback = value;
        }

        void FileColorSpacesWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void FileColorSpacesWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void FileColorSpacesWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.addButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_file_tooltip")));
                p.deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_files_tooltip")));
                _widgetUpdate();
            }
        }

        void FileColorSpacesWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                p.deleteButtonGroup->clearButtons();
                p.itemLayout->clearChildren();
                for (const auto& i : p.fileColorSpaces)
                {
                    auto button = UI::ListButton::create(context);
                    button->setText((!i.first.empty() ? i.first : _getText(DJV_TEXT("av_ocio_files_default"))) + ":");
                    button->setRightText(!i.second.empty() ? i.second : _getText(DJV_TEXT("av_ocio_files_none")));
                    button->setRightIcon("djvIconArrowSmallRight");
                    p.buttonGroup->addButton(button);

                    auto deleteButton = UI::ToolButton::create(context);
                    deleteButton->setIcon("djvIconClear");
                    deleteButton->setVisible(p.deleteEnabled);
                    deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_file_tooltip")));
                    p.deleteButtonGroup->addButton(deleteButton);

                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(button);
                    hLayout->setStretch(button, UI::RowStretch::Expand);
                    hLayout->addChild(deleteButton);
                    p.itemLayout->addChild(hLayout);
                }
            }
        }

        struct FileColorSpaceWidget::Private
        {
            std::string file;
            std::vector<std::string> colorSpaces;
            std::map<std::string, std::string> fileColorSpaces;

            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;

            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > fileColorSpacesObserver;
        };

        void FileColorSpaceWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::FileColorSpaceWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Swatch);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<FileColorSpaceWidget>(std::dynamic_pointer_cast<FileColorSpaceWidget>(shared_from_this()));
            p.backButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->backCallback)
                        {
                            widget->_p->backCallback();
                        }
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto fileColorSpaces = widget->_p->fileColorSpaces;
                            fileColorSpaces[widget->_p->file] =
                                value >= 0 && value < static_cast<int>(widget->_p->colorSpaces.size()) ?
                                widget->_p->colorSpaces[value] :
                                std::string();
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            ocioSystem->setFileColorSpaces(fileColorSpaces);
                        }
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
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

            p.fileColorSpacesObserver = MapObserver<std::string, std::string>::create(
                ocioSystem->observeFileColorSpaces(),
                [weak](const std::map<std::string, std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileColorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        FileColorSpaceWidget::FileColorSpaceWidget() :
            _p(new Private)
        {}

        FileColorSpaceWidget::~FileColorSpaceWidget()
        {}

        std::shared_ptr<FileColorSpaceWidget> FileColorSpaceWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FileColorSpaceWidget>(new FileColorSpaceWidget);
            out->_init(context);
            return out;
        }

        void FileColorSpaceWidget::setFile(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.file)
                return;
            p.file = value;
            _widgetUpdate();
        }

        void FileColorSpaceWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void FileColorSpaceWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void FileColorSpaceWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void FileColorSpaceWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_view")));
                _widgetUpdate();
            }
        }

        void FileColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.backButton->setText(
                    !p.file.empty() ?
                    p.file :
                    _getText(DJV_TEXT("av_ocio_files_default")));
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.colorSpaces.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.colorSpaces[i].empty() ?
                        p.colorSpaces[i] :
                        _getText(DJV_TEXT("av_ocio_files_none"));
                    items.emplace_back(item);
                }
                int index = -1;
                const auto i = p.fileColorSpaces.find(p.file);
                if (i != p.fileColorSpaces.end())
                {
                    const auto j = std::find(p.colorSpaces.begin(), p.colorSpaces.end(), i->second);
                    if (j != p.colorSpaces.end())
                    {
                        index = static_cast<int>(j - p.colorSpaces.begin());
                    }
                }
                p.listWidget->setItems(items, index);
            }
        }

    } // namespace ViewApp
} // namespace djv

