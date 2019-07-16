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

#include <djvViewApp/FileSettingsWidget.h>

#include <djvViewApp/FileSettings.h>

#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct FileSettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> autoDetectSequencesButton;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ValueObserver<bool> > autoDetectSequencesObserver;
        };

        void FileSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::FileSettingsWidget");

            p.autoDetectSequencesButton = UI::ToggleButton::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.autoDetectSequencesButton);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<FileSettingsWidget>(std::dynamic_pointer_cast<FileSettingsWidget>(shared_from_this()));
            p.autoDetectSequencesButton->setCheckedCallback(
                [weak, context](bool value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                    {
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setAutoDetectSequences(value);
                        }
                    }
                }
            });

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
                if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                {
                    p.autoDetectSequencesObserver = ValueObserver<bool>::create(
                        fileSettings->observeAutoDetectSequences(),
                        [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->autoDetectSequencesButton->setChecked(value);
                        }
                    });
                }
            }
        }

        FileSettingsWidget::FileSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<FileSettingsWidget> FileSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<FileSettingsWidget>(new FileSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string FileSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("File");
        }

        std::string FileSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("File");
        }

        std::string FileSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void FileSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.autoDetectSequencesButton, _getText(DJV_TEXT("Auto-detect file sequences")) + ":");
        }

    } // namespace ViewApp
} // namespace djv

