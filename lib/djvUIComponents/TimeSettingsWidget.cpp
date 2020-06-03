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

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class TimeUnitsWidget : public Widget
            {
                DJV_NON_COPYABLE(TimeUnitsWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                TimeUnitsWidget();

            public:
                static std::shared_ptr<TimeUnitsWidget> create(const std::shared_ptr<Context>&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                void _widgetUpdate();
                void _currentItemUpdate();

                DJV_PRIVATE();
            };

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
                Widget::_initEvent(event);
                if (event.getData().textChanged)
                {
                    _widgetUpdate();
                }
            }

            void TimeUnitsWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->clearItems();
                for (auto i : Time::getUnitsEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    p.comboBox->addItem(_getText(ss.str()));
                }
                _currentItemUpdate();
            }

            void TimeUnitsWidget::_currentItemUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->setCurrentItem(static_cast<int>(p.timeUnits));
            }

        } // namespace

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

        std::string TimeSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_general_section_time");
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
            if (event.getData().textChanged)
            {
                p.layout->setText(p.timeUnitsWidget, _getText(DJV_TEXT("settings_general_time_units")) + ":");
            }
        }

    } // namespace UI
} // namespace djv

