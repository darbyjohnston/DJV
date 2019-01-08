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

#include <djvViewLib/IViewSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct IViewSystem::Private
        {
        };

        void IViewSystem::_init(const std::string & name, Context * context)
        {
            ISystem::_init(name, context);
        }
            
        IViewSystem::IViewSystem() :
            _p(new Private)
        {}

        IViewSystem::~IViewSystem()
        {}
        
        std::map<std::string, std::shared_ptr<UI::Action> > IViewSystem::getActions()
        {
            return std::map<std::string, std::shared_ptr<UI::Action> >();
        }
        
        std::shared_ptr<UI::Menu> IViewSystem::createMenu()
        {
            return nullptr;
        }

        std::string IViewSystem::getMenuSortKey() const
        {
            return getName();
        }

        std::shared_ptr<IToolWidget> IViewSystem::createToolWidget()
        {
            return nullptr;
        }

        std::string IViewSystem::getToolWidgetSortKey() const
        {
            return getName();
        }

        bool IViewSystem::isToolWidgetVisible() const
        {
            return false;
        }

        std::shared_ptr<UI::Widget> IViewSystem::createSettingsWidget()
        {
            return nullptr;
        }

        std::string IViewSystem::getSettingsSortKey() const
        {
            return getName();
        }

        void IViewSystem::setCurrentMedia(const std::shared_ptr<Media> &)
        {}

    } // namespace ViewLib
} // namespace djv
