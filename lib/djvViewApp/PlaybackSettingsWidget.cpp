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

#include <djvViewApp/PlaybackSettingsWidget.h>

#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/CheckButton.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckButton> pipButton;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ValueObserver<bool> > pipObserver;
        };

        void PlaybackSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSettingsWidget");

            p.pipButton = UI::CheckButton::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.pipButton);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<PlaybackSettingsWidget>(std::dynamic_pointer_cast<PlaybackSettingsWidget>(shared_from_this()));
            p.pipButton->setCheckedCallback(
                [weak, context](bool value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                    {
                        if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                        {
                            playbackSettings->setPIP(value);
                        }
                    }
                }
            });

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                {
                    p.pipObserver = ValueObserver<bool>::create(
                        playbackSettings->observePIP(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->pipButton->setChecked(value);
                        }
                    });
                }
            }
        }

        PlaybackSettingsWidget::PlaybackSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettingsWidget> PlaybackSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<PlaybackSettingsWidget>(new PlaybackSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PlaybackSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Playback");
        }

        std::string PlaybackSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("DJV");
        }

        std::string PlaybackSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void PlaybackSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.pipButton, _getText(DJV_TEXT("Show timeline PIP")) + ":");
        }

    } // namespace ViewApp
} // namespace djv

