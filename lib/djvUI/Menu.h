// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Style.h>

#include <djvSystem/IObject.h>

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

        //! Popup menu widget.
        class Menu : public System::IObject
        {
            DJV_NON_COPYABLE(Menu);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Menu();

        public:
            ~Menu() override;

            //! Create a new menu.
            static std::shared_ptr<Menu> create(const std::shared_ptr<System::Context>&);

            //! \name Icon
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeIcon() const;

            void setIcon(const std::string&);

            ///@}

            //! \name Text
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeText() const;

            void setText(const std::string&);

            ///@}

            //! \name Actions
            ///@{

            void addAction(const std::shared_ptr<Action>&);
            void removeAction(const std::shared_ptr<Action>&);
            void clearActions();

            ///@}

            //! \name Layout
            ///@{

            void addSeparator();

            ///@}

            //! \name Options
            ///@{

            MetricsRole getMinimumSizeRole() const;
            ColorRole getBackgroundColorRole() const;

            void setMinimumSizeRole(MetricsRole);
            void setBackgroundColorRole(ColorRole);

            ///@}

            //! \name Open and Close
            ///@{

            std::shared_ptr<Widget> popup(const glm::vec2&);
            std::shared_ptr<Widget> popup(const std::weak_ptr<Button::Menu>& button);
            std::shared_ptr<Widget> popup(const std::weak_ptr<Button::Menu>& button, const std::weak_ptr<Widget>& anchor);

            bool isOpen() const;
            
            void close();
            void setCloseCallback(const std::function<void(void)>&);

            ///@}

        private:
            void _createWidgets();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

