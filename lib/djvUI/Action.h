// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <string>

namespace djv
{
    namespace UI
    {
        class Shortcut;
        struct ShortcutData;
        struct ShortcutDataPair;

        //! This class provides an action.
        class Action : public std::enable_shared_from_this<Action>
        {
            DJV_NON_COPYABLE(Action);

        protected:
            void _init();
            Action();

        public:
            virtual ~Action();

            static std::shared_ptr<Action> create();
            static std::shared_ptr<Action> create(const std::string& text);
            static std::shared_ptr<Action> create(
                const std::string& text,
                const std::string& textBrief);

            //! \name Button Type
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<ButtonType> > observeButtonType() const;

            void setButtonType(ButtonType);

            ///@}

            //! \name Checked State
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeChecked() const;
            
            void setChecked(bool);

            ///@}

            //! \name Icon
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeIcon() const;
            
            void setIcon(const std::string&);
            void setCheckedIcon(const std::string&);

            ///@}

            //! \name Text
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeText() const;
            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeTextBrief() const;
            
            void setText(const std::string&);
            void setTextBrief(const std::string&);

            ///@}

            //! \name Font
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeFont() const;
            
            void setFont(const std::string&);

            ///@}

            //! \name Keyboard Shortcuts
            ///@{

            std::shared_ptr<Core::Observer::IListSubject<std::shared_ptr<Shortcut> > > observeShortcuts() const;
            
            void setShortcuts(const ShortcutDataPair&);
            void setShortcut(const std::shared_ptr<Shortcut>&);
            void setShortcut(int key);
            void setShortcut(int key, int keyModifiers);
            void addShortcut(const ShortcutData&);
            void addShortcut(const std::shared_ptr<Shortcut>&);
            void addShortcut(int key);
            void addShortcut(int key, int keyModifiers);
            void clearShortcuts();

            ///@}

            //! \name Enabled State
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeEnabled() const;
            
            void setEnabled(bool);

            ///@}

            //! \name Tooltip
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::string> > observeTooltip() const;
            
            void setTooltip(const std::string&);

            ///@}

            //! \name Auto-Repeat
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeAutoRepeat() const;
            
            void setAutoRepeat(bool);

            ///@}

            //! \name Callbacks
            ///@{

            void setClickedCallback(const std::function<void(void)>&);
            void setCheckedCallback(const std::function<void(bool)>&);
            void doClick();

            ///@}

        private:
            void _iconUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
