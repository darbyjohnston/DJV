// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewSettingsWidget.h>

#include <djvViewApp/ViewSettings.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Speed.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewSettingsWidget::Private
        {
            std::shared_ptr<UI::ComboBox> scrollWheelZoomSpeedComboBox;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<ScrollWheelZoomSpeed> > scrollWheelZoomSpeedObserver;
        };

        void ViewSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewSettingsWidget");

            p.scrollWheelZoomSpeedComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.scrollWheelZoomSpeedComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ViewSettingsWidget>(std::dynamic_pointer_cast<ViewSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.scrollWheelZoomSpeedComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
                            {
                                viewSettings->setScrollWheelZoomSpeed(static_cast<ScrollWheelZoomSpeed>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
            {
                p.scrollWheelZoomSpeedObserver = ValueObserver<ScrollWheelZoomSpeed>::create(
                    viewSettings->observeScrollWheelZoomSpeed(),
                    [weak](ScrollWheelZoomSpeed value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scrollWheelZoomSpeedComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        ViewSettingsWidget::ViewSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ViewSettingsWidget> ViewSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ViewSettingsWidget>(new ViewSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ViewSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_view_section_view");
        }

        std::string ViewSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_view");
        }

        std::string ViewSettingsWidget::getSettingsSortKey() const
        {
            return "C";
        }

        void ViewSettingsWidget::setSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setSizeGroup(value);
        }

        void ViewSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.scrollWheelZoomSpeedComboBox, _getText(DJV_TEXT("scroll_wheel_zoom_speed")) + ":");
            _widgetUpdate();
        }

        void ViewSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<std::string> items;
                for (auto i : getScrollWheelZoomSpeedEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                p.scrollWheelZoomSpeedComboBox->setItems(items);
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
                {
                    p.scrollWheelZoomSpeedComboBox->setCurrentItem(static_cast<int>(viewSettings->observeScrollWheelZoomSpeed()->get()));
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

