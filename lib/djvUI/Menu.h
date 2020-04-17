// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Style.h>

#include <djvCore/IObject.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        class Action;
        class Widget;

        namespace Button
        {
            class Menu;
        
        } // namespace Button

        //! This class provides a popup menu widget.
        class Menu : public Core::IObject
        {
            DJV_NON_COPYABLE(Menu);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            Menu();

        public:
            virtual ~Menu();

            static std::shared_ptr<Menu> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<std::string> > observeIcon() const;
            std::shared_ptr<Core::IValueSubject<std::string> > observeText() const;
            void setIcon(const std::string &);
            void setText(const std::string &);

            void addAction(const std::shared_ptr<Action> &);
            void removeAction(const std::shared_ptr<Action> &);
            void clearActions();
            void addSeparator();

            MetricsRole getMinimumSizeRole() const;
            void setMinimumSizeRole(MetricsRole);

            ColorRole getBackgroundRole() const;
            void setBackgroundRole(ColorRole);

            std::shared_ptr<Widget> popup(const glm::vec2 &);
            std::shared_ptr<Widget> popup(const std::weak_ptr<Button::Menu> & button);
            std::shared_ptr<Widget> popup(const std::weak_ptr<Button::Menu> & button, const std::weak_ptr<Widget> & anchor);

            bool isOpen() const;
            void close();

            void setCloseCallback(const std::function<void(void)> &);

        private:
            void _createWidgets();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

