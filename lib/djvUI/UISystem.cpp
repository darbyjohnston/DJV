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

#include <djvUI/UISystem.h>

#include <djvUI/AVSettings.h>
#include <djvUI/DialogSystem.h>
#include <djvUI/ColorSpaceSettings.h>
#include <djvUI/GeneralSettings.h>
#include <djvUI/FontSettings.h>
#include <djvUI/IconSystem.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/Window.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct UISystem::Private
        {
            uint16_t dpi = AV::dpiDefault;
            std::shared_ptr<Style::Style> style;
            std::shared_ptr<ValueObserver<UI::Style::Palette> > paletteObserver;
            std::shared_ptr<ValueObserver<UI::Style::Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > fontObserver;
        };

        void UISystem::_init(uint16_t dpi, Context * context)
        {
            ISystem::_init("djv::UI::UISystem", context);

            DJV_PRIVATE_PTR();
            p.dpi = dpi;

            addDependency(context->getSystemT<AV::AVSystem>());

            auto settingsSystem = Settings::System::create(context);
            Settings::AV::create(context);
            Settings::ColorSpace::create(context);
            Settings::General::create(context);
            Settings::Font::create(context);
            auto styleSettings = Settings::Style::create(context);

            auto iconSystem = IconSystem::create(context);

            p.style = Style::Style::create(dpi, context);
            
            auto dialogSystem = DialogSystem::create(context);

            addDependency(settingsSystem);
            addDependency(iconSystem);
            addDependency(dialogSystem);

            auto weak = std::weak_ptr<UISystem>(std::dynamic_pointer_cast<UISystem>(shared_from_this()));
            p.paletteObserver = ValueObserver<UI::Style::Palette>::create(
                styleSettings->observeCurrentPalette(),
                [weak](const UI::Style::Palette & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->style->setPalette(value);
                }
            });

            p.metricsObserver = ValueObserver<UI::Style::Metrics>::create(
                styleSettings->observeCurrentMetrics(),
                [weak](const UI::Style::Metrics & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->style->setMetrics(value);
                }
            });

            p.fontObserver = ValueObserver<std::string>::create(
                styleSettings->observeCurrentFont(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->style->setFont(value);
                }
            });
        }

        UISystem::UISystem() :
            _p(new Private)
        {}

        UISystem::~UISystem()
        {}

        std::shared_ptr<UISystem> UISystem::create(uint16_t dpi, Context * context)
        {
            auto out = std::shared_ptr<UISystem>(new UISystem);
            out->_init(dpi, context);
            return out;
        }

        uint16_t UISystem::getDPI() const
        {
            return _p->dpi;
        }

        const std::shared_ptr<Style::Style> & UISystem::getStyle() const
        {
            return _p->style;
        }

    } // namespace UI
} // namespace djv

