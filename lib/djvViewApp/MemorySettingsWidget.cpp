// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MemorySettingsWidget.h>

#include <djvViewApp/FileSettings.h>

#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/FormLayout.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvSystem/Context.h>

#include <djvCore/OSFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MemoryCacheEnabledWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> button;

            std::shared_ptr<Observer::Value<bool> > enabledObserver;
        };

        void MemoryCacheEnabledWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheEnabledWidget");

            p.button = UI::ToggleButton::create(context);

            addChild(p.button);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.button->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheEnabled(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheEnabledWidget>(
                std::dynamic_pointer_cast<MemoryCacheEnabledWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.enabledObserver = Observer::Value<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->button->setChecked(value);
                        }
                    });
            }
        }

        MemoryCacheEnabledWidget::MemoryCacheEnabledWidget() :
            _p(new Private)
        {}

        MemoryCacheEnabledWidget::~MemoryCacheEnabledWidget()
        {}

        std::shared_ptr<MemoryCacheEnabledWidget> MemoryCacheEnabledWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheEnabledWidget>(new MemoryCacheEnabledWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheEnabledWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->button->getMinimumSize() + getMargin().getSize(style));
        }

        void MemoryCacheEnabledWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const Math::BBox2f g2 = UI::Layout::getAlign(
                g,
                p.button->getMinimumSize(),
                p.button->getHAlign(),
                p.button->getVAlign());
            _p->button->setGeometry(g2);
        }

        struct MemoryCacheSizeWidget::Private
        {
            std::shared_ptr<UI::Numeric::IntSlider> slider;
            std::shared_ptr<UI::Text::Label> label;
            std::shared_ptr<UI::HorizontalLayout> layout;

            std::shared_ptr<Observer::Value<int> > sizeObserver;
        };

        void MemoryCacheSizeWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheSizeWidget");

            p.slider = UI::Numeric::IntSlider::create(context);
            p.slider->setRange(Math::IntRange(1, OS::getRAMSize() / Memory::gigabyte));
            p.label = UI::Text::Label::create(context);
            p.label->setTextHAlign(UI::TextHAlign::Left);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.slider);
            p.layout->setStretch(p.slider, UI::RowStretch::Expand);
            p.layout->addChild(p.label);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.slider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheSize(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheSizeWidget>(
                std::dynamic_pointer_cast<MemoryCacheSizeWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.sizeObserver = Observer::Value<int>::create(
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

        std::shared_ptr<MemoryCacheSizeWidget> MemoryCacheSizeWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheSizeWidget>(new MemoryCacheSizeWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheSizeWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void MemoryCacheSizeWidget::_layoutEvent(System::Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void MemoryCacheSizeWidget::_initEvent(System::Event::Init& event)
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
            std::shared_ptr<UI::FormLayout> layout;
        };

        void MemorySettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::MemorySettingsWidget");

            p.enabledWidget = MemoryCacheEnabledWidget::create(context);
            p.sizeWidget = MemoryCacheSizeWidget::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.enabledWidget);
            p.layout->addChild(p.sizeWidget);
            addChild(p.layout);
        }

        MemorySettingsWidget::MemorySettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<MemorySettingsWidget> MemorySettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MemorySettingsWidget>(new MemorySettingsWidget);
            out->_init(context);
            return out;
        }

        std::string MemorySettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_memory");
        }

        std::string MemorySettingsWidget::getSettingsSortKey() const
        {
            return "H";
        }

        void MemorySettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void MemorySettingsWidget::_initEvent(System::Event::Init& event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.enabledWidget, _getText(DJV_TEXT("settings_memory_cache_enabled")) + ":");
                p.layout->setText(p.sizeWidget, _getText(DJV_TEXT("settings_memory_cache_size")) + ":");
            }
        }

    } // namespace ViewApp
} // namespace djv

