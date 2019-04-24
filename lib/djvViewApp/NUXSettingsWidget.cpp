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

#include <djvUI/CheckButton.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckButton> nuxButton;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ValueObserver<bool> > nuxObserver;
        };

        void NUXSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::NUXSettingsWidget");

            p.nuxButton = UI::CheckButton::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.nuxButton);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<NUXSettingsWidget>(std::dynamic_pointer_cast<NUXSettingsWidget>(shared_from_this()));
            p.nuxButton->setCheckedCallback(
                [weak, context](bool value)
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
                            widget->_p->nuxButton->setChecked(value);
                        }
                    });
                }
            }
        }

        NUXSettingsWidget::NUXSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSettingsWidget> NUXSettingsWidget::create(Context * context)
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
            return DJV_TEXT("DJV");
        }

        std::string NUXSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void NUXSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.nuxButton, _getText(DJV_TEXT("Enable NUX on startup")) + ":");
        }

    } // namespace ViewApp
} // namespace djv

