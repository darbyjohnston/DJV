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

#include <djvViewApp/NUXSystem.h>

#include <djvViewApp/NUXSystemSettings.h>

#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/DisplaySettingsWidget.h>

#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSystem::Private
        {
            std::shared_ptr<NUXSystemSettings> settings;
            bool nux = false;
            size_t index = 0;
            std::shared_ptr<UI::Label> titleLabel;
            std::map<std::string, std::shared_ptr<UI::Label> > labels;
            std::map < std::string, std::shared_ptr<UI::PushButton> > buttons;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::shared_ptr<ValueObserver<bool> > nuxObserver;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void NUXSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::NUXSystem", context);

            DJV_PRIVATE_PTR();
            p.settings = NUXSystemSettings::create(context);

            auto languageWidget = UI::LanguageWidget::create(context);
            languageWidget->setFontSizeRole(UI::MetricsRole::FontLarge);
            auto displaySizeWidget = UI::DisplaySizeWidget::create(context);
            displaySizeWidget->setFontSizeRole(UI::MetricsRole::FontLarge);
            auto displayPaletteWidget = UI::DisplayPaletteWidget::create(context);
            displayPaletteWidget->setFontSizeRole(UI::MetricsRole::FontLarge);

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontTitle);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);

            p.labels["Language"] = UI::Label::create(context);
            p.labels["DisplaySize"] = UI::Label::create(context);
            p.labels["DisplayPalette"] = UI::Label::create(context);
            for (const auto& i : p.labels)
            {
                i.second->setFontSizeRole(UI::MetricsRole::FontLarge);
                i.second->setTextHAlign(UI::TextHAlign::Left);
            }

            p.buttons["Next"] = UI::PushButton::create(context);
            p.buttons["Prev"] = UI::PushButton::create(context);
            p.buttons["Finish"] = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            //layout->setSpacing(UI::MetricsRole::SpacingLarge);
            layout->setHAlign(UI::HAlign::Center);
            layout->setVAlign(UI::VAlign::Center);
            layout->addChild(p.titleLabel);
            layout->addSeparator();
            auto soloLayout = UI::SoloLayout::create(context);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["Language"]);
            vLayout->addChild(languageWidget);
            soloLayout->addChild(vLayout);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["DisplaySize"]);
            vLayout->addChild(displaySizeWidget);
            soloLayout->addChild(vLayout);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["DisplayPalette"]);
            vLayout->addChild(displayPaletteWidget);
            soloLayout->addChild(vLayout);
            layout->addChild(soloLayout);
            layout->setStretch(soloLayout, UI::RowStretch::Expand);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            auto hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->addChild(p.buttons["Prev"]);
            hLayout2->addChild(p.buttons["Next"]);
            hLayout->addChild(hLayout2);
            hLayout->addExpander();
            hLayout->addChild(p.buttons["Finish"]);
            layout->addSeparator();
            layout->addChild(hLayout);

            p.overlay = UI::Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setCapturePointer(false);
            p.overlay->setFadeIn(false);
            p.overlay->setVisible(true);
            p.overlay->setBackgroundRole(UI::ColorRole::Background);
            p.overlay->addChild(layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<NUXSystem>(std::dynamic_pointer_cast<NUXSystem>(shared_from_this()));
            p.buttons["Next"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto system = weak.lock())
                {
                    if (system->_p->index < 3)
                    {
                        ++system->_p->index;
                        soloLayout->setCurrentIndex(system->_p->index);
                    }
                    system->_widgetUpdate();
                }
            });

            p.buttons["Prev"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto system = weak.lock())
                {
                    if (system->_p->index > 0)
                    {
                        --system->_p->index;
                        soloLayout->setCurrentIndex(system->_p->index);
                    }
                    system->_widgetUpdate();
                }
            });

            p.buttons["Finish"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto system = weak.lock())
                {
                    system->_p->settings->setNUX(false);
                    system->_p->overlay->hide();
                }
            });

            p.nuxObserver = ValueObserver<bool>::create(
                p.settings->observeNUX(),
                [weak](bool value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->nux = value;
                }
            });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        NUXSystem::NUXSystem() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSystem> NUXSystem::create(Context * context)
        {
            auto out = std::shared_ptr<NUXSystem>(new NUXSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<UI::Widget> NUXSystem::getNUXWidget() const
        {
            DJV_PRIVATE_PTR();
            return p.nux ? p.overlay : nullptr;
        }

        void NUXSystem::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.buttons["Next"]->setEnabled(p.index < 2);
            p.buttons["Prev"]->setEnabled(p.index > 0);
        }

        void NUXSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();

            std::stringstream ss;
            ss << _getText(DJV_TEXT("DJV "));
            ss << " " << DJV_VERSION;
            p.titleLabel->setText(ss.str());

            p.labels["Language"]->setText(_getText(DJV_TEXT("Choose your language")) + ": ");
            p.labels["DisplaySize"]->setText(_getText(DJV_TEXT("Choose a display size")) + ": ");
            p.labels["DisplayPalette"]->setText(_getText(DJV_TEXT("Choose a palette")) + ": ");
            p.buttons["Next"]->setText(_getText(DJV_TEXT("Next")));
            p.buttons["Prev"]->setText(_getText(DJV_TEXT("Previous")));
            p.buttons["Finish"]->setText(_getText(DJV_TEXT("Finish")));
        }

    } // namespace ViewApp
} // namespace djv

