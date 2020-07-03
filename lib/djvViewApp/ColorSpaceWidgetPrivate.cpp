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
#include <djvUI/ToolButton.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceMainWidget::Private
        {
            AV::OCIO::ConfigData configData;
            AV::OCIO::DisplayData displayData;
            AV::OCIO::ViewData viewData;

            std::shared_ptr<UI::ListWidget> configListWidget;
            std::shared_ptr<UI::SearchBox> configSearchBox;
            std::shared_ptr<UI::PopupButton> configPopupButton;

            std::shared_ptr<UI::ListWidget> displayListWidget;
            std::shared_ptr<UI::SearchBox> displaySearchBox;
            std::shared_ptr<UI::PopupButton> displayPopupButton;

            std::shared_ptr<UI::ListWidget> viewListWidget;
            std::shared_ptr<UI::SearchBox> viewSearchBox;
            std::shared_ptr<UI::PopupButton> viewPopupButton;

            std::shared_ptr<UI::ListButton> editConfigsButton;
            std::shared_ptr<UI::ListButton> fileFormatsButton;

            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> editConfigsCallback;
            std::function<void()> fileFormatsCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigData> > configDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::DisplayData> > displayDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::ViewData> > viewDataObserver;
        };

        void ColorSpaceMainWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceMainWidget");

            p.configListWidget = UI::ListWidget::create(UI::ButtonType::Exclusive, context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.configListWidget);
            p.configSearchBox = UI::SearchBox::create(context);
            p.configPopupButton = UI::PopupButton::create(context);
            p.configPopupButton->setPopupIcon("djvIconPopupMenu");
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            vLayout->addSeparator();
            vLayout->addChild(p.configSearchBox);
            p.configPopupButton->addChild(vLayout);

            p.displayListWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.displayListWidget);
            p.displaySearchBox = UI::SearchBox::create(context);
            p.displayPopupButton = UI::PopupButton::create(context);
            p.displayPopupButton->setPopupIcon("djvIconPopupMenu");
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            vLayout->addSeparator();
            vLayout->addChild(p.displaySearchBox);
            p.displayPopupButton->addChild(vLayout);

            p.viewListWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.viewListWidget);
            p.viewSearchBox = UI::SearchBox::create(context);
            p.viewPopupButton = UI::PopupButton::create(context);
            p.viewPopupButton->setPopupIcon("djvIconPopupMenu");
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            vLayout->addSeparator();
            vLayout->addChild(p.viewSearchBox);
            p.viewPopupButton->addChild(vLayout);

            p.editConfigsButton = UI::ListButton::create(context);
            p.editConfigsButton->setRightIcon("djvIconArrowSmallRight");

            p.fileFormatsButton = UI::ListButton::create(context);
            p.fileFormatsButton->setRightIcon("djvIconArrowSmallRight");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.configPopupButton);
            p.formLayout->addChild(p.displayPopupButton);
            p.formLayout->addChild(p.viewPopupButton);
            p.layout->addChild(p.formLayout);
            p.layout->addSeparator();
            p.layout->addChild(p.editConfigsButton);
            p.layout->addChild(p.fileFormatsButton);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.configListWidget->setExclusiveCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentConfig(value);
                    }
                });
            p.displayListWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentDisplay(value);
                    }
                });
            p.viewListWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentView(value);
                    }
                });

            auto weak = std::weak_ptr<ColorSpaceMainWidget>(std::dynamic_pointer_cast<ColorSpaceMainWidget>(shared_from_this()));
            p.configSearchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configListWidget->setFilter(value);
                    }
                });
            p.displaySearchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displayListWidget->setFilter(value);
                    }
                });
            p.viewSearchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewListWidget->setFilter(value);
                    }
                });

            p.editConfigsButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->editConfigsCallback)
                        {
                            widget->_p->editConfigsCallback();
                        }
                    }

                });

            p.fileFormatsButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->fileFormatsCallback)
                        {
                            widget->_p->fileFormatsCallback();
                        }
                    }

                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configDataObserver = ValueObserver<AV::OCIO::ConfigData>::create(
                ocioSystem->observeConfigData(),
                [weak](const AV::OCIO::ConfigData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.displayDataObserver = ValueObserver<AV::OCIO::DisplayData>::create(
                ocioSystem->observeDisplayData(),
                [weak](const AV::OCIO::DisplayData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displayData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.viewDataObserver = ValueObserver<AV::OCIO::ViewData>::create(
                ocioSystem->observeViewData(),
                [weak](const AV::OCIO::ViewData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewData = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceMainWidget::ColorSpaceMainWidget() :
            _p(new Private)
        {}

        ColorSpaceMainWidget::~ColorSpaceMainWidget()
        {}

        std::shared_ptr<ColorSpaceMainWidget> ColorSpaceMainWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceMainWidget>(new ColorSpaceMainWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceMainWidget::setEditConfigsCallback(const std::function<void()>& value)
        {
            _p->editConfigsCallback = value;
        }

        void ColorSpaceMainWidget::setFileFormatsCallback(const std::function<void()>& value)
        {
            _p->fileFormatsCallback = value;
        }

        void ColorSpaceMainWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceMainWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceMainWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.formLayout->setText(p.configPopupButton, _getText(DJV_TEXT("widget_color_space_config")) + ":");
                p.formLayout->setText(p.displayPopupButton, _getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.formLayout->setText(p.viewPopupButton, _getText(DJV_TEXT("widget_color_space_view")) + ":");

                p.editConfigsButton->setText(_getText(DJV_TEXT("widget_color_space_edit_configs")));
                p.fileFormatsButton->setText(_getText(DJV_TEXT("widget_color_space_edit_file_formats")));

                _widgetUpdate();
            }
        }

        void ColorSpaceMainWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.configData.names.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = p.configData.names[i];
                    item.tooltip = p.configData.fileNames[i];
                    items.emplace_back(item);
                }
                p.configListWidget->setItems(items, p.configData.current);
                p.configPopupButton->setText(
                    p.configData.current >= 0 &&
                    p.configData.current < static_cast<int>(p.configData.names.size()) ?
                    p.configData.names[p.configData.current] :
                    _getText(DJV_TEXT("av_ocio_display_none")));

                items.clear();
                for (size_t i = 0; i < p.displayData.names.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.displayData.names[i].empty() ?
                        p.displayData.names[i] :
                        _getText(DJV_TEXT("av_ocio_display_none"));
                    items.emplace_back(item);
                }
                p.displayListWidget->setItems(items, p.displayData.current);
                p.displayPopupButton->setText(
                    p.displayData.current >= 0 &&
                    p.displayData.current < static_cast<int>(p.displayData.names.size()) &&
                    !p.displayData.names[p.displayData.current].empty() ?
                    p.displayData.names[p.displayData.current] :
                    _getText(DJV_TEXT("av_ocio_display_none")));

                items.clear();
                for (size_t i = 0; i < p.viewData.names.size(); ++i)
                {
                    UI::ListItem item;
                    item.text = !p.viewData.names[i].empty() ?
                        p.viewData.names[i] :
                        _getText(DJV_TEXT("av_ocio_display_none"));
                    items.emplace_back(item);
                }
                p.viewListWidget->setItems(items, p.viewData.current);
                p.viewPopupButton->setText(
                    p.viewData.current >= 0 &&
                    p.viewData.current < static_cast<int>(p.viewData.names.size()) &&
                    !p.viewData.names[p.viewData.current].empty() ?
                    p.viewData.names[p.viewData.current] :
                    _getText(DJV_TEXT("av_ocio_display_none")));
            }
        }

        namespace
        {
            class ConfigWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ConfigWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                ConfigWidget();

            public:
                ~ConfigWidget() override;

                static std::shared_ptr<ConfigWidget> create(const std::shared_ptr<Context>&);

                void setText(const std::string&);
                void setDelete(bool);
                void setDeleteCallback(const std::function<void()>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();

                std::shared_ptr<UI::Label> _label;
                std::shared_ptr<UI::ToolButton> _deleteButton;
                std::shared_ptr<UI::HorizontalLayout> _layout;
            };

            void ConfigWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _label = UI::Label::create(context);
                _label->setTextHAlign(UI::TextHAlign::Left);
                _label->setMargin(UI::MetricsRole::MarginSmall);

                _deleteButton = UI::ToolButton::create(context);
                _deleteButton->setIcon("djvIconClearSmall");
                _deleteButton->setInsideMargin(UI::MetricsRole::None);
                _deleteButton->setVisible(false);

                _layout = UI::HorizontalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                _layout->addChild(_label);
                _layout->setStretch(_label, UI::RowStretch::Expand);
                _layout->addChild(_deleteButton);
                addChild(_layout);
            }

            ConfigWidget::ConfigWidget()
            {}

            ConfigWidget:: ~ConfigWidget()
            {}

            std::shared_ptr<ConfigWidget> ConfigWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ConfigWidget>(new ConfigWidget);
                out->_init(context);
                return out;
            }

            void ConfigWidget::setText(const std::string& value)
            {
                _label->setText(value);
            }

            void ConfigWidget::setDelete(bool value)
            {
                _deleteButton->setVisible(value);
            }

            void ConfigWidget::setDeleteCallback(const std::function<void()>& value)
            {
                _deleteButton->setClickedCallback(value);
            }

            void ConfigWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void ConfigWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void ConfigWidget::_initEvent(Event::Init& event)
            {
                _deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_config_tooltip")));

            }

            void ConfigWidget::_widgetUpdate()
            {

            }

        } // namespace

        struct ColorSpaceEditConfigsWidget::Private
        {
            AV::OCIO::ConfigData data;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");
            bool edit = false;

            std::shared_ptr<UI::ListButton> backButton;
            std::vector<std::shared_ptr<ConfigWidget> > configWidgets;
            std::shared_ptr<UI::ToolButton> addButton;
            std::shared_ptr<UI::ToolButton> deleteButton;
            std::shared_ptr<UI::VerticalLayout> itemLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;

            std::function<void()> backCallback;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigData> > configDataObserver;

            void initWidget(
                const std::shared_ptr<ConfigWidget>& widget,
                size_t                               index,
                const std::string&                   name,
                const std::string&                   fileName,
                const std::weak_ptr<Context>&        contextWeak);
        };

        void ColorSpaceEditConfigsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceEditConfigsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

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
            p.itemLayout = UI::VerticalLayout::create(context);
            p.itemLayout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
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

            auto weak = std::weak_ptr<ColorSpaceEditConfigsWidget>(std::dynamic_pointer_cast<ColorSpaceEditConfigsWidget>(shared_from_this()));
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
                        widget->_p->edit = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configDataObserver = ValueObserver<AV::OCIO::ConfigData>::create(
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

        ColorSpaceEditConfigsWidget::ColorSpaceEditConfigsWidget() :
            _p(new Private)
        {}

        ColorSpaceEditConfigsWidget::~ColorSpaceEditConfigsWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<ColorSpaceEditConfigsWidget> ColorSpaceEditConfigsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceEditConfigsWidget>(new ColorSpaceEditConfigsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceEditConfigsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceEditConfigsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceEditConfigsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceEditConfigsWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_edit_configs")));
                p.addButton->setTooltip(_getText(DJV_TEXT("widget_color_space_add_config_tooltip")));
                p.deleteButton->setTooltip(_getText(DJV_TEXT("widget_color_space_delete_configs_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorSpaceEditConfigsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto contextWeak = std::weak_ptr<Context>(context);
                p.itemLayout->clearChildren();
                const size_t widgetSize = p.configWidgets.size();
                size_t i = 0;
                for (; i < p.data.names.size() && i < widgetSize; ++i)
                {
                    const auto& configWidget = p.configWidgets[i];
                    p.initWidget(configWidget, i, p.data.names[i], p.data.fileNames[i], contextWeak);
                }
                for (; i < p.data.names.size(); ++i)
                {
                    auto configWidget = ConfigWidget::create(context);
                    p.initWidget(configWidget, i, p.data.names[i], p.data.fileNames[i], contextWeak);
                    p.itemLayout->addChild(configWidget);
                }
                for (; i < widgetSize; ++i)
                {
                    auto configWidget = p.configWidgets.begin() + p.data.names.size();
                    p.itemLayout->removeChild(*configWidget);
                    p.configWidgets.erase(configWidget);
                }
            }
        }

        void ColorSpaceEditConfigsWidget::Private::initWidget(
            const std::shared_ptr<ConfigWidget>& widget,
            size_t index,
            const std::string& name,
            const std::string& fileName,
            const std::weak_ptr<Context>& contextWeak)
        {
            widget->setText(name);
            widget->setDelete(edit);
            widget->setBackgroundRole(0 == index % 2 ? UI::ColorRole::None : UI::ColorRole::Trough);
            widget->setTooltip(fileName);
            widget->setDeleteCallback(
                [contextWeak, index]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->removeConfig(static_cast<int>(index));
                    }
                });
        }

        struct ColorSpaceFileFormatsWidget::Private
        {
            std::shared_ptr<UI::ListButton> backButton;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void()> backCallback;
        };

        void ColorSpaceFileFormatsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceFileFormatsWidget");

            p.backButton = UI::ListButton::create(context);
            p.backButton->setIcon("djvIconArrowSmallLeft");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.backButton);
            p.layout->addSeparator();
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceFileFormatsWidget>(std::dynamic_pointer_cast<ColorSpaceFileFormatsWidget>(shared_from_this()));
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
        }

        ColorSpaceFileFormatsWidget::ColorSpaceFileFormatsWidget() :
            _p(new Private)
        {}

        ColorSpaceFileFormatsWidget::~ColorSpaceFileFormatsWidget()
        {}

        std::shared_ptr<ColorSpaceFileFormatsWidget> ColorSpaceFileFormatsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceFileFormatsWidget>(new ColorSpaceFileFormatsWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceFileFormatsWidget::setBackCallback(const std::function<void()>& value)
        {
            _p->backCallback = value;
        }

        void ColorSpaceFileFormatsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceFileFormatsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceFileFormatsWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.backButton->setText(_getText(DJV_TEXT("widget_color_space_edit_file_formats")));
                _widgetUpdate();
            }
        }

        void ColorSpaceFileFormatsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
            }
        }

    } // namespace ViewApp
} // namespace djv

