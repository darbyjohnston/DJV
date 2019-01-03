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

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/BBox.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace UI
    {
        class Action;
        class Window;

        enum class MenuType
        {
            Default,
            ComboBox
        };

        //! This class provides a menu widget.
        //!
        //! \todo Add sub-menus.
        class Menu : public std::enable_shared_from_this<Menu>
        {
            DJV_NON_COPYABLE(Menu);

        protected:
            void _init(Core::Context *);
            Menu();

        public:
            virtual ~Menu();
            static std::shared_ptr<Menu> create(Core::Context *);
            static std::shared_ptr<Menu> create(const std::string & name, Core::Context *);

            std::shared_ptr<Core::IValueSubject<std::string> > getMenuName() const;
            void setMenuName(const std::string &);

            void addAction(const std::shared_ptr<Action> &);
            void addSeparator();
            void clearActions();

            void popup(const std::shared_ptr<Window> &, const glm::vec2 &, MenuType = MenuType::Default);
            void popup(const std::shared_ptr<Window> &, const std::weak_ptr<Widget> & button, MenuType = MenuType::Default);
            void popup(const std::shared_ptr<Window> &, const std::weak_ptr<Widget> & button, const std::weak_ptr<Widget> & anchor, MenuType = MenuType::Default);
            void hide();

            void setCloseCallback(const std::function<void(void)> &);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv

