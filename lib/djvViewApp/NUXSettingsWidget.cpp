//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/NUXSettingsWidget.h>

#include <djvViewApp/NUXSettings.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> nuxCheckBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > nuxObserver;
        };

        void NUXSettingsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::NUXSettingsWidget");

            p.nuxCheckBox = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.nuxCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<NUXSettingsWidget>(std::dynamic_pointer_cast<NUXSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.nuxCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                            {
                                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                                {
                                    nuxSettings->setNUX(value);
                                }
                            }
                        }
                    }
                });

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
                if (auto nuxSettings = settingsSystem->getSettingsT<NUXSettings>())
                {
                    p.nuxObserver = ValueObserver<bool>::create(
                        nuxSettings->observeNUX(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->nuxCheckBox->setChecked(value);
                        }
                    });
                }
            }
        }

        NUXSettingsWidget::NUXSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSettingsWidget> NUXSettingsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<NUXSettingsWidget>(new NUXSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string NUXSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("NUX");
        }

        std::string NUXSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string NUXSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void NUXSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.nuxCheckBox->setText(_getText(DJV_TEXT("Enable new-user experience on startup")));
        }

    } // namespace ViewApp
} // namespace djv

