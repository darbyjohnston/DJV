// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/TimeSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct TimeUnitsWidget::Private
        {
            AV::Time::Units timeUnits = AV::Time::Units::First;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
        };

        void TimeUnitsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::TimeUnitsWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            _widgetUpdate();

            auto weak = std::weak_ptr<TimeUnitsWidget>(std::dynamic_pointer_cast<TimeUnitsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto system = context->getSystemT<AV::AVSystem>())
                        {
                            system->setTimeUnits(static_cast<AV::Time::Units>(value));
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = Observer::Value<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_currentItemUpdate();
                    }
                });
        }

        TimeUnitsWidget::TimeUnitsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<TimeUnitsWidget> TimeUnitsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TimeUnitsWidget>(new TimeUnitsWidget);
            out->_init(context);
            return out;
        }

        float TimeUnitsWidget::getHeightForWidth(float value) const
        {
            return _p->comboBox->getHeightForWidth(value);
        }

        void TimeUnitsWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void TimeUnitsWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void TimeUnitsWidget::_initEvent(System::Event::Init& event)
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
            for (auto i : AV::Time::getUnitsEnums())
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

        namespace Settings
        {
            struct TimeWidget::Private
            {
                std::shared_ptr<TimeUnitsWidget> timeUnitsWidget;
                std::shared_ptr<UI::FormLayout> layout;
            };

            void TimeWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::Settings::TimeWidget");

                p.timeUnitsWidget = TimeUnitsWidget::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.timeUnitsWidget);
                addChild(p.layout);
            }

            TimeWidget::TimeWidget() :
                _p(new Private)
            {}

            std::shared_ptr<TimeWidget> TimeWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<TimeWidget>(new TimeWidget);
                out->_init(context);
                return out;
            }

            std::string TimeWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_general");
            }

            std::string TimeWidget::getSettingsSortKey() const
            {
                return "0";
            }

            void TimeWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
            {
                _p->layout->setLabelSizeGroup(value);
            }

            void TimeWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.timeUnitsWidget, _getText(DJV_TEXT("settings_general_time_units")) + ":");
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

