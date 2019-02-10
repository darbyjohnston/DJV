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

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvUIComponents/FileBrowserDialog.h>
#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/UISystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct UIComponentsSystem::Private
        {
            std::vector<std::shared_ptr<ISystem> > systems;
            std::shared_ptr<Settings::FileBrowser> fileBrowserSettings;
        };

        void UIComponentsSystem::_init(int dpi, Context * context)
        {
            ISystem::_init("djv::UI::UIComponentsSystem", context);

            DJV_PRIVATE_PTR();

            p.fileBrowserSettings = Settings::FileBrowser::create(context);

			p.systems.push_back(UISystem::create(dpi, context));
            p.systems.push_back(FileBrowser::DialogSystem::create(context));
        }

		UIComponentsSystem::UIComponentsSystem() :
            _p(new Private)
        {}

		UIComponentsSystem::~UIComponentsSystem()
        {
            DJV_PRIVATE_PTR();
            while (p.systems.size())
            {
                auto system = p.systems.back();
                system->setParent(nullptr);
                p.systems.pop_back();
            }
        }

        std::shared_ptr<UIComponentsSystem> UIComponentsSystem::create(int dpi, Context * context)
        {
            auto out = std::shared_ptr<UIComponentsSystem>(new UIComponentsSystem);
            out->_init(dpi, context);
            return out;
        }

        const std::shared_ptr<Settings::FileBrowser> & UIComponentsSystem::getFileBrowserSettings() const
        {
            return _p->fileBrowserSettings;
        }

    } // namespace UI
} // namespace djv

