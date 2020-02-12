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

#include <djvUIComponents/IOSettingsWidget.h>

#include <djvUIComponents/IOSettings.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IOThreadsSettingsWidget::Private
        {
            std::shared_ptr<IntSlider> threadCountSlider;
            std::shared_ptr<FormLayout> layout;
            std::shared_ptr<ValueObserver<size_t> > threadCountObserver;
        };

        void IOThreadsSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::IOThreadsSettingsWidget");

            p.threadCountSlider = IntSlider::create(context);
            p.threadCountSlider->setRange(IntRange(2, 64));

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.threadCountSlider);
            addChild(p.layout);

            auto weak = std::weak_ptr<IOThreadsSettingsWidget>(std::dynamic_pointer_cast<IOThreadsSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.threadCountSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto ioSettings = settingsSystem->getSettingsT<Settings::IO>())
                            {
                                ioSettings->setThreadCount(static_cast<size_t>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto ioSettings = settingsSystem->getSettingsT<Settings::IO>())
            {
                p.threadCountObserver = ValueObserver<size_t>::create(
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

        IOThreadsSettingsWidget::IOThreadsSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<IOThreadsSettingsWidget> IOThreadsSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IOThreadsSettingsWidget>(new IOThreadsSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string IOThreadsSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_io_section_threads");
        }

        std::string IOThreadsSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_io");
        }

        std::string IOThreadsSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void IOThreadsSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.threadCountSlider, _getText(DJV_TEXT("thread_count")) + ":");
        }

    } // namespace UI
} // namespace djv

