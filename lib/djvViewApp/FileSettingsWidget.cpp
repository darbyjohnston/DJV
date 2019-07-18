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

#include <djvUI/FloatSlider.h>
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
        struct SequenceSettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> button;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ValueObserver<bool> > observer;
        };

        void SequenceSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SequenceSettingsWidget");

            p.button = UI::ToggleButton::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.button);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<SequenceSettingsWidget>(std::dynamic_pointer_cast<SequenceSettingsWidget>(shared_from_this()));
            p.button->setCheckedCallback(
                [weak, context](bool value)
            {
                if (auto widget = weak.lock())
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                    {
                        fileSettings->setAutoDetectSequences(value);
                    }
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.observer = ValueObserver<bool>::create(
                    fileSettings->observeAutoDetectSequences(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->button->setChecked(value);
                    }
                });
            }
        }

        SequenceSettingsWidget::SequenceSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<SequenceSettingsWidget> SequenceSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<SequenceSettingsWidget>(new SequenceSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string SequenceSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Sequences");
        }

        std::string SequenceSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("File");
        }

        std::string SequenceSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void SequenceSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.button, _getText(DJV_TEXT("Auto-detect file sequences")) + ":");
        }

        struct CacheSettingsWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> enabledButton;
            std::shared_ptr<UI::FloatSlider> maxSlider;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ValueObserver<bool> > enabledObserver;
            std::shared_ptr<ValueObserver<float> > maxObserver;
        };

        void CacheSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::CacheSettingsWidget");

            p.enabledButton = UI::ToggleButton::create(context);

            p.maxSlider = UI::FloatSlider::create(context);
            p.maxSlider->setRange(FloatRange(1.0f, 64.f));

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.enabledButton);
            p.formLayout->addChild(p.maxSlider);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<CacheSettingsWidget>(
                std::dynamic_pointer_cast<CacheSettingsWidget>(shared_from_this()));
            p.enabledButton->setCheckedCallback(
                [weak, context](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheEnabled(value);
                        }
                    }
                });
            p.maxSlider->setValueCallback(
                [weak, context](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheMax(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.enabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->enabledButton->setChecked(value);
                        }
                    });
                p.maxObserver = ValueObserver<float>::create(
                    fileSettings->observeCacheMax(),
                    [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->maxSlider->setValue(value);
                        }
                    });
            }
        }

        CacheSettingsWidget::CacheSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<CacheSettingsWidget> CacheSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<CacheSettingsWidget>(new CacheSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string CacheSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Memory Cache");
        }

        std::string CacheSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("File");
        }

        std::string CacheSettingsWidget::getSettingsSortKey() const
        {
            return "A";
        }

        void CacheSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.enabledButton, _getText(DJV_TEXT("Enabled")) + ":");
            p.formLayout->setText(p.maxSlider, _getText(DJV_TEXT("Gigabytes")) + ":");
        }

    } // namespace ViewApp
} // namespace djv

