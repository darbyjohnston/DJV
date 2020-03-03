//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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
                AV::TimeUnits timeUnits = AV::TimeUnits::First;
                std::shared_ptr<ComboBox> comboBox;
                std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
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
                                system->setTimeUnits(static_cast<AV::TimeUnits>(value));
                            }
                        }
                    });

                if (auto avSystem = context->getSystemT<AV::AVSystem>())
                {
                    p.timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                        avSystem->observeTimeUnits(),
                        [weak](AV::TimeUnits value)
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
                _widgetUpdate();
            }

            void TimeUnitsWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                p.comboBox->clearItems();
                for (auto i : AV::getTimeUnitsEnums())
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

        void TimeSettingsWidget::setSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setSizeGroup(value);
        }

        void TimeSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.timeUnitsWidget, _getText(DJV_TEXT("settings_general_time_units")) + ":");
        }

    } // namespace UI
} // namespace djv

