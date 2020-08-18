// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MemorySettingsWidget.h>

#include <djvViewApp/FileSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/OS.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MemoryCacheEnabledWidget::Private
        {
            std::shared_ptr<UI::CheckBox> checkBox;

            std::shared_ptr<ValueObserver<bool> > enabledObserver;
        };

        void MemoryCacheEnabledWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheEnabledWidget");

            p.checkBox = UI::CheckBox::create(context);

            addChild(p.checkBox);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.checkBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheEnabled(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheEnabledWidget>(
                std::dynamic_pointer_cast<MemoryCacheEnabledWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.enabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->checkBox->setChecked(value);
                        }
                    });
            }
        }

        MemoryCacheEnabledWidget::MemoryCacheEnabledWidget() :
            _p(new Private)
        {}

        MemoryCacheEnabledWidget::~MemoryCacheEnabledWidget()
        {}

        std::shared_ptr<MemoryCacheEnabledWidget> MemoryCacheEnabledWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheEnabledWidget>(new MemoryCacheEnabledWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheEnabledWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->checkBox->getMinimumSize() + getMargin().getSize(style));
        }

        void MemoryCacheEnabledWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->checkBox->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void MemoryCacheEnabledWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.checkBox->setText(_getText(DJV_TEXT("settings_memory_cache_enabled")));
            }
        }

        struct MemoryCacheSizeWidget::Private
        {
            std::shared_ptr<UI::IntSlider> slider;
            std::shared_ptr<UI::Label> label;
            std::shared_ptr<UI::HorizontalLayout> layout;

            std::shared_ptr<ValueObserver<int> > sizeObserver;
        };

        void MemoryCacheSizeWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheSizeWidget");

            p.slider = UI::IntSlider::create(context);
            p.slider->setRange(IntRange(1, OS::getRAMSize() / Memory::gigabyte));
            p.label = UI::Label::create(context);
            p.label->setTextHAlign(UI::TextHAlign::Left);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.slider);
            p.layout->setStretch(p.slider, UI::RowStretch::Expand);
            p.layout->addChild(p.label);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.slider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheSize(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheSizeWidget>(
                std::dynamic_pointer_cast<MemoryCacheSizeWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.sizeObserver = ValueObserver<int>::create(
                    fileSettings->observeCacheSize(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->slider->setValue(value);
                        }
                    });
            }
        }

        MemoryCacheSizeWidget::MemoryCacheSizeWidget() :
            _p(new Private)
        {}

        MemoryCacheSizeWidget::~MemoryCacheSizeWidget()
        {}

        std::shared_ptr<MemoryCacheSizeWidget> MemoryCacheSizeWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheSizeWidget>(new MemoryCacheSizeWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheSizeWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void MemoryCacheSizeWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void MemoryCacheSizeWidget::_initEvent(Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                std::stringstream ss;
                ss << Memory::Unit::GB;
                p.label->setText(_getText(ss.str()));
            }
        }

        struct MemorySettingsWidget::Private
        {
            std::shared_ptr<MemoryCacheEnabledWidget> enabledWidget;
            std::shared_ptr<MemoryCacheSizeWidget> sizeWidget;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void MemorySettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::MemorySettingsWidget");

            p.enabledWidget = MemoryCacheEnabledWidget::create(context);
            p.sizeWidget = MemoryCacheSizeWidget::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.enabledWidget);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.sizeWidget);
            p.layout->addChild(p.formLayout);
            addChild(p.layout);
        }

        MemorySettingsWidget::MemorySettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<MemorySettingsWidget> MemorySettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<MemorySettingsWidget>(new MemorySettingsWidget);
            out->_init(context);
            return out;
        }

        std::string MemorySettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_memory_cache");
        }

        std::string MemorySettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_memory");
        }

        std::string MemorySettingsWidget::getSettingsSortKey() const
        {
            return "H";
        }

        void MemorySettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->formLayout->setLabelSizeGroup(value);
        }

        void MemorySettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.formLayout->setText(p.sizeWidget, _getText(DJV_TEXT("settings_memory_cache_size")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

