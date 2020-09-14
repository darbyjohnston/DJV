// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/TimeSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TimeFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TimeUnitsWidget::Private
        {
            Time::Units timeUnits = Time::Units::First; 
            std::shared_ptr<ComboBox> comboBox;
            std::shared_ptr<ValueObserver<Time::Units> > timeUnitsObserver;
        };

        void TimeUnitsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TimeUnitsWidget");

            p.comboBox = ComboBox::create(context);
            addChild(p.comboBox);

            _widgetUpdate();

            auto weak = std::weak_ptr<TimeUnitsWidget>(std::dynamic_pointer_cast<TimeUnitsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = context->getSystemT<AV::AVSystem>())
                        {
                            system->setTimeUnits(static_cast<Time::Units>(value));
                        }
                    }
                });

            if (auto avSystem = context->getSystemT<AV::AVSystem>())
            {
                p.timeUnitsObserver = ValueObserver<Time::Units>::create(
                    avSystem->observeTimeUnits(),
                    [weak](Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_currentItemUpdate();
                    }
                });
            }
        }

        TimeUnitsWidget::TimeUnitsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TimeUnitsWidget> TimeUnitsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimeUnitsWidget>(new TimeUnitsWidget);
            out->_init(context);
            return out;
        }

        float TimeUnitsWidget::getHeightForWidth(float value) const
        {
            return _p->comboBox->getHeightForWidth(value);
        }

        void TimeUnitsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void TimeUnitsWidget::_layoutEvent(Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void TimeUnitsWidget::_initEvent(Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void TimeUnitsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (auto i : Time::getUnitsEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.comboBox->setItems(items);
            p.comboBox->setCurrentItem(static_cast<int>(p.timeUnits));
        }

        void TimeUnitsWidget::_currentItemUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->setCurrentItem(static_cast<int>(p.timeUnits));
        }

        struct TimeSettingsWidget::Private
        {
            std::shared_ptr<TimeUnitsWidget> timeUnitsWidget;
            std::shared_ptr<FormLayout> layout;
        };

        void TimeSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::TimeSettingsWidget");

            p.timeUnitsWidget = TimeUnitsWidget::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.timeUnitsWidget);
            addChild(p.layout);
        }

        TimeSettingsWidget::TimeSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TimeSettingsWidget> TimeSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimeSettingsWidget>(new TimeSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string TimeSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_general");
        }

        std::string TimeSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void TimeSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void TimeSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.layout->setText(p.timeUnitsWidget, _getText(DJV_TEXT("settings_general_time_units")) + ":");
            }
        }

    } // namespace UI
} // namespace djv

