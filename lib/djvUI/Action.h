// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        class Shortcut;

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

            std::shared_ptr<Core::IValueSubject<ButtonType> > observeButtonType() const;
            void setButtonType(ButtonType);

            std::shared_ptr<Core::IValueSubject<bool> > observeClicked() const;
            void doClicked();

            std::shared_ptr<Core::IValueSubject<bool> > observeChecked() const;
            void setChecked(bool);

            std::shared_ptr<Core::IValueSubject<std::string> > observeIcon() const;
            void setIcon(const std::string&);
            void setCheckedIcon(const std::string&);

            std::shared_ptr<Core::IValueSubject<std::string> > observeText() const;
            void setText(const std::string &);

            std::shared_ptr<Core::IValueSubject<std::string> > observeFont() const;
            void setFont(const std::string &);

            std::shared_ptr<Core::IValueSubject<MetricsRole> > observeInsideMargin() const;
            void setInsideMargin(MetricsRole);

            std::shared_ptr<Core::IListSubject<std::shared_ptr<Shortcut> > > observeShortcuts() const;
            void setShortcut(const std::shared_ptr<Shortcut> &);
            void setShortcut(int key);
            void setShortcut(int key, int keyModifiers);
            void addShortcut(const std::shared_ptr<Shortcut> &);
            void addShortcut(int key);
            void addShortcut(int key, int keyModifiers);
            void clearShortcuts();

            std::shared_ptr<Core::IValueSubject<bool> > observeEnabled() const;
            void setEnabled(bool);

            std::shared_ptr<Core::IValueSubject<std::string> > observeTooltip() const;
            void setTooltip(const std::string&);

            std::shared_ptr<Core::IValueSubject<bool> > observeAutoRepeat() const;
            void setAutoRepeat(bool);

        private:
            void _iconUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
