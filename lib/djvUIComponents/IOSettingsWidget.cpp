// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/IOSettingsWidget.h>

#include <djvUIComponents/IOSettings.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>

#include <djvMath/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct IOThreadsWidget::Private
            {
                std::shared_ptr<UI::Numeric::IntSlider> threadCountSlider;
                std::shared_ptr<UI::FormLayout> layout;
                std::shared_ptr<Observer::Value<size_t> > threadCountObserver;
            };

            void IOThreadsWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::IOThreadsWidget");

                p.threadCountSlider = UI::Numeric::IntSlider::create(context);
                p.threadCountSlider->setRange(Math::IntRange(2, 64));

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.threadCountSlider);
                addChild(p.layout);

                auto weak = std::weak_ptr<IOThreadsWidget>(std::dynamic_pointer_cast<IOThreadsWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.threadCountSlider->setValueCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                if (auto ioSettings = settingsSystem->getSettingsT<Settings::IO>())
                                {
                                    ioSettings->setThreadCount(static_cast<size_t>(value));
                                }
                            }
                        }
                    });

                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                if (auto ioSettings = settingsSystem->getSettingsT<Settings::IO>())
                {
                    p.threadCountObserver = Observer::Value<size_t>::create(
                        ioSettings->observeThreadCount(),
                        [weak](size_t value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->threadCountSlider->setValue(static_cast<int>(value));
                            }
                        });
                }
            }

            IOThreadsWidget::IOThreadsWidget() :
                _p(new Private)
            {}

            std::shared_ptr<IOThreadsWidget> IOThreadsWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<IOThreadsWidget>(new IOThreadsWidget);
                out->_init(context);
                return out;
            }

            std::string IOThreadsWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_io");
            }

            std::string IOThreadsWidget::getSettingsSortKey() const
            {
                return "d";
            }

            void IOThreadsWidget::setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>& value)
            {
                _p->layout->setLabelSizeGroup(value);
            }

            void IOThreadsWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("settings_io_thread_count")) + ":");
                }
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

