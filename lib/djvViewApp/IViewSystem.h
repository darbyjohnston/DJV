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

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>

#include <functional>
#include <map>

namespace djv
{
    namespace UI
    {
        class Action;
        class ISettingsWidget;
        class Menu;
    
        namespace MDI
        {
            class Canvas;

        } // namespace MDI
    } // namespace UI

    namespace ViewApp
    {
        class MDIWidget;

        //! This struct provides menu data.
        struct MenuData
        {
            std::shared_ptr<UI::Menu> menu;
            std::string sortKey;
        };

        //! This class provides the base functionality for systems.
        class IViewSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(IViewSystem);

        protected:
            void _init(const std::string & name, const std::shared_ptr<Core::Context>&);
            IViewSystem();

        public:
            ~IViewSystem() override;

            virtual std::map<std::string, std::shared_ptr<UI::Action> > getActions() const;
            virtual MenuData getMenu() const;
            virtual std::vector<std::shared_ptr<UI::ISettingsWidget> > createSettingsWidgets() const;

            void setCanvas(const std::shared_ptr<UI::MDI::Canvas>&);

        protected:
            virtual void _openWidget(const std::string&, const std::shared_ptr<MDIWidget>&);
            virtual void _closeWidget(const std::string&);

            const std::map<std::string, std::shared_ptr<MDIWidget> >& _getWidgets() const;
            const std::map<std::string, Core::BBox2f>& _getWidgetGeom() const;
            void _setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

