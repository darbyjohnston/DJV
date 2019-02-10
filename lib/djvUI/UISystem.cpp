//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/DialogSystem.h>
#include <djvUI/GeneralSettings.h>
#include <djvUI/FontSettings.h>
#include <djvUI/IconSystem.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>
#include <djvUI/StyleSettings.h>

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
            int dpi = AV::dpiDefault;
            std::vector<std::shared_ptr<ISystem> > systems;
            std::shared_ptr<Settings::General> generalSettings;
            std::shared_ptr<Settings::Font> fontSettings;
            std::shared_ptr<Settings::Style> styleSettings;
            std::shared_ptr<Style::Style> style;
            std::shared_ptr<ValueObserver<UI::Style::Palette> > paletteObserver;
            std::shared_ptr<ValueObserver<UI::Style::Metrics> > metricsObserver;
            std::shared_ptr<ValueObserver<std::string> > fontObserver;
        };

        void UISystem::_init(int dpi, Context * context)
        {
            ISystem::_init("djv::UI::UISystem", context);

            DJV_PRIVATE_PTR();
            p.dpi = dpi;

            p.systems.push_back(AV::AVSystem::create(context));

            p.systems.push_back(Settings::System::create(context));
            p.generalSettings = Settings::General::create(context);
            p.fontSettings = Settings::Font::create(context);
            p.styleSettings = Settings::Style::create(context);

            p.style = Style::Style::create(dpi, context);
            
            p.systems.push_back(IconSystem::create(context));
            p.systems.push_back(DialogSystem::create(context));

            auto weak = std::weak_ptr<UISystem>(std::dynamic_pointer_cast<UISystem>(shared_from_this()));
            p.paletteObserver = ValueObserver<UI::Style::Palette>::create(
                p.styleSettings->observeCurrentPalette(),
                [weak](const UI::Style::Palette & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->style->setPalette(value);
                }
            });
            p.metricsObserver = ValueObserver<UI::Style::Metrics>::create(
                p.styleSettings->observeCurrentMetrics(),
                [weak](const UI::Style::Metrics & value)
            {
                if (auto system = weak.lock())
                {
                    system->_p->style->setMetrics(value);
                }
            });
            p.fontObserver = ValueObserver<std::string>::create(
                p.styleSettings->observeCurrentFont(),
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
        {
            DJV_PRIVATE_PTR();
            while (p.systems.size())
            {
                auto system = p.systems.back();
                system->setParent(nullptr);
                p.systems.pop_back();
            }
        }

        std::shared_ptr<UISystem> UISystem::create(int dpi, Context * context)
        {
            auto out = std::shared_ptr<UISystem>(new UISystem);
            out->_init(dpi, context);
            return out;
        }

        int UISystem::getDPI() const
        {
            return _p->dpi;
        }

        const std::shared_ptr<Settings::General> & UISystem::getGeneralSettings() const
        {
            return _p->generalSettings;
        }

        const std::shared_ptr<Settings::Font> & UISystem::getFontSettings() const
        {
            return _p->fontSettings;
        }

        const std::shared_ptr<Settings::Style> & UISystem::getStyleSettings() const
        {
            return _p->styleSettings;
        }

        const std::shared_ptr<Style::Style> & UISystem::getStyle() const
        {
            return _p->style;
        }

    } // namespace UI
} // namespace djv

