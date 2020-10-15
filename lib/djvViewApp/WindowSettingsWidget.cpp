// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/WindowSettingsWidget.h>

#include <djvViewApp/WindowSettings.h>

#include <djvDesktopApp/GLFWSystem.h>

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/LineEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct WindowGeometrySettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> restorePosButton;
            std::shared_ptr<UI::ToggleButton> restoreSizeButton;
            std::shared_ptr<Observer::Value<bool> > restorePosObserver;
            std::shared_ptr<Observer::Value<bool> > restoreSizeObserver;
            std::shared_ptr<UI::FormLayout> layout;
        };

        void WindowGeometrySettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::WindowGeometrySettingsWidget");

            p.restorePosButton = UI::ToggleButton::create(context);
            p.restoreSizeButton = UI::ToggleButton::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.restorePosButton);
            p.layout->addChild(p.restoreSizeButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<WindowGeometrySettingsWidget>(std::dynamic_pointer_cast<WindowGeometrySettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.restorePosButton->setCheckedCallback(
                [weak, contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setRestorePos(value);
                        }
                    }
                }
            });

            p.restoreSizeButton->setCheckedCallback(
                [weak, contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setRestoreSize(value);
                        }
                    }
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.restorePosObserver = Observer::Value<bool>::create(
                    windowSettings->observeRestorePos(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->restorePosButton->setChecked(value);
                    }
                });

                p.restoreSizeObserver = Observer::Value<bool>::create(
                    windowSettings->observeRestoreSize(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->restoreSizeButton->setChecked(value);
                    }
                });
            }
        }

        WindowGeometrySettingsWidget::WindowGeometrySettingsWidget() :
            _p(new Private)
        {}

        WindowGeometrySettingsWidget::~WindowGeometrySettingsWidget()
        {}

        std::shared_ptr<WindowGeometrySettingsWidget> WindowGeometrySettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<WindowGeometrySettingsWidget>(new WindowGeometrySettingsWidget);
            out->_init(context);
            return out;
        }

        std::string WindowGeometrySettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_window_title");
        }

        std::string WindowGeometrySettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void WindowGeometrySettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void WindowGeometrySettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.restorePosButton, _getText(DJV_TEXT("settings_window_restore_pos")) + ":");
                p.layout->setText(p.restoreSizeButton, _getText(DJV_TEXT("settings_window_restore_size")) + ":");
            }
        }

        struct FullscreenMonitorSettingsWidget::Private
        {
            std::vector<std::string> monitorNames;
            int monitor = 0;
            std::shared_ptr<UI::ComboBox> monitorComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<Observer::List<Desktop::MonitorInfo> > monitorInfoObserver;
            std::shared_ptr<Observer::Value<int> > monitorObserver;
        };

        void FullscreenMonitorSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::FullscreenMonitorSettingsWidget");

            p.monitorComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.monitorComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<FullscreenMonitorSettingsWidget>(std::dynamic_pointer_cast<FullscreenMonitorSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.monitorComboBox->setCallback(
                [weak, contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setFullScreenMonitor(value);
                        }
                    }
                }
            });

            auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>();
            p.monitorInfoObserver = Observer::List<Desktop::MonitorInfo>::create(
                glfwSystem->observeMonitorInfo(),
                [weak](const std::vector<Desktop::MonitorInfo>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->monitorNames.clear();
                    for (const auto& i : value)
                    {
                        widget->_p->monitorNames.push_back(i.name);
                    }
                    widget->_widgetUpdate();
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.monitorObserver = Observer::Value<int>::create(
                    windowSettings->observeFullScreenMonitor(),
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->monitor = value;
                        widget->_widgetUpdate();
                    }
                });
            }
        }

        FullscreenMonitorSettingsWidget::FullscreenMonitorSettingsWidget() :
            _p(new Private)
        {}

        FullscreenMonitorSettingsWidget::~FullscreenMonitorSettingsWidget()
        {}

        std::shared_ptr<FullscreenMonitorSettingsWidget> FullscreenMonitorSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<FullscreenMonitorSettingsWidget>(new FullscreenMonitorSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string FullscreenMonitorSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_window_title");
        }

        std::string FullscreenMonitorSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void FullscreenMonitorSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void FullscreenMonitorSettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.monitorComboBox, _getText(DJV_TEXT("settings_window_section_fullscreen_monitor")) + ":");
            }
        }

        void FullscreenMonitorSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.monitorComboBox->setItems(p.monitorNames);
            p.monitorComboBox->setCurrentItem(p.monitor);
        }
        
        struct BackgroundImageSettingsWidget::Private
        {
            std::string fileName;
            AV::ThumbnailSystem::ImageFuture imageFuture;
            std::shared_ptr<Image::Data> image;
            bool scale = false;
            bool colorize = false;
            System::File::Path fileBrowserPath = System::File::Path(".");

            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::Text::LineEdit> lineEdit;
            std::shared_ptr<UI::ToolButton> openButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::ToggleButton> scaleButton;
            std::shared_ptr<UI::ToggleButton> colorizeButton;
            std::shared_ptr<UIComponents::FileBrowser::Dialog> fileBrowserDialog;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<std::string> > backgroundImageObserver;
            std::shared_ptr<Observer::Value<bool> > backgroundImageScaleObserver;
            std::shared_ptr<Observer::Value<bool> > backgroundImageColorizeObserver;
        };

        void BackgroundImageSettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::BackgroundImageSettingsWidget");

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setMargin(UI::MetricsRole::MarginSmall);
            Render2D::ImageOptions options;
            options.alphaBlend = Render2D::AlphaBlend::Straight;
            p.imageWidget->setImageOptions(options);
            p.imageWidget->setImageSizeRole(UI::MetricsRole::TextColumn);
            p.imageWidget->setHAlign(UI::HAlign::Center);
            p.imageWidget->setVAlign(UI::VAlign::Center);

            p.lineEdit = UI::Text::LineEdit::create(context);

            p.openButton = UI::ToolButton::create(context);
            p.openButton->setIcon("djvIconFile");
            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.scaleButton = UI::ToggleButton::create(context);
            p.colorizeButton = UI::ToggleButton::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.imageWidget);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout ->addChild(p.lineEdit);
            hLayout->setStretch(p.lineEdit, UI::RowStretch::Expand);
            hLayout->addChild(p.openButton);
            hLayout->addChild(p.closeButton);
            p.layout->addChild(hLayout);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.scaleButton);
            p.formLayout->addChild(p.colorizeButton);
            p.layout->addChild(p.formLayout);
            addChild(p.layout);

            _widgetUpdate();
            _imageUpdate();

            auto weak = std::weak_ptr<BackgroundImageSettingsWidget>(std::dynamic_pointer_cast<BackgroundImageSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.lineEdit->setTextEditCallback(
                [weak, contextWeak](const std::string& value, UI::TextEditReason)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                            {
                                windowSettings->setBackgroundImage(value);
                            }
                        }
                    }
                });

            p.openButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->fileBrowserDialog)
                            {
                                widget->_p->fileBrowserDialog->close();
                            }
                            widget->_p->fileBrowserDialog = UIComponents::FileBrowser::Dialog::create(UI::SelectionType::Single, context);
                            auto io = context->getSystemT<AV::IO::IOSystem>();
                            widget->_p->fileBrowserDialog->setFileExtensions(io->getFileExtensions());
                            widget->_p->fileBrowserDialog->setPath(widget->_p->fileBrowserPath);
                            widget->_p->fileBrowserDialog->setCallback(
                                [weak, contextWeak](const std::vector<System::File::Info>& value)
                                {
                                    if (auto context = contextWeak.lock())
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            const auto temp = value;
                                            if (widget->_p->fileBrowserDialog)
                                            {
                                                widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                                widget->_p->fileBrowserDialog->close();
                                                widget->_p->fileBrowserDialog.reset();
                                            }
                                            if (!temp.empty())
                                            {
                                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                                if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                                                {
                                                    windowSettings->setBackgroundImage(std::string(temp[0].getPath()));
                                                }
                                            }
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        if (widget->_p->fileBrowserDialog)
                                        {
                                            widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                            widget->_p->fileBrowserDialog->close();
                                            widget->_p->fileBrowserDialog.reset();
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->show();
                        }
                    }
                });

            p.closeButton->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImage(std::string());
                        }
                    }
                });

            p.scaleButton->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImageScale(value);
                        }
                    }
                });

            p.colorizeButton->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImageColorize(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.backgroundImageObserver = Observer::Value<std::string>::create(
                    windowSettings->observeBackgroundImage(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->fileName = value;
                            widget->_widgetUpdate();
                            widget->_imageUpdate();
                        }
                    });

                p.backgroundImageScaleObserver = Observer::Value<bool>::create(
                    windowSettings->observeBackgroundImageScale(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scale = value;
                            widget->_widgetUpdate();
                        }
                    });

                p.backgroundImageColorizeObserver = Observer::Value<bool>::create(
                    windowSettings->observeBackgroundImageColorize(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorize = value;
                            widget->_widgetUpdate();
                        }
                    });
            }
        }

        BackgroundImageSettingsWidget::BackgroundImageSettingsWidget() :
            _p(new Private)
        {}

        BackgroundImageSettingsWidget::~BackgroundImageSettingsWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<BackgroundImageSettingsWidget> BackgroundImageSettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<BackgroundImageSettingsWidget>(new BackgroundImageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string BackgroundImageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_window_section_background_image");
        }

        std::string BackgroundImageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_window_title");
        }

        std::string BackgroundImageSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void BackgroundImageSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
        {
            _p->formLayout->setLabelSizeGroup(value);
        }

        void BackgroundImageSettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.openButton->setTooltip(_getText(DJV_TEXT("settings_window_open_file_browser")));
                p.closeButton->setTooltip(_getText(DJV_TEXT("settings_window_clear_background_image")));
                p.formLayout->setText(p.scaleButton, _getText(DJV_TEXT("settings_window_scale_to_fit")) + ":");
                p.formLayout->setText(p.colorizeButton, _getText(DJV_TEXT("settings_window_colorize")) + ":");
            }
            _imageUpdate();
        }

        void BackgroundImageSettingsWidget::_updateEvent(System::Event::Update&)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFuture.future.valid() &&
                p.imageFuture.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.image = p.imageFuture.future.get();
                }
                catch (const std::exception& e)
                {
                    p.image.reset();
                    _log(e.what(), System::LogLevel::Error);
                }
                p.imageWidget->setImage(p.image);
            }
        }

        void BackgroundImageSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageWidget->setImageColorRole(p.colorize ? UI::ColorRole::Button : UI::ColorRole::None);
            p.lineEdit->setText(p.fileName);
            p.scaleButton->setChecked(p.scale);
            p.colorizeButton->setChecked(p.colorize);
        }

        void BackgroundImageSettingsWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!p.fileName.empty())
                {
                    const auto& style = _getStyle();
                    const float s = style->getMetric(UI::MetricsRole::TextColumn);
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    p.imageFuture = thumbnailSystem->getImage(p.fileName, Image::Size(s, s));
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

