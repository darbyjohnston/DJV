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

#include <djvUI/Enum.h>
#include <djvUI/Style.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;

        } // namespace FileSystem
    } // namespace Core

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

            std::shared_ptr<Core::IValueSubject<ButtonType> > getButtonType() const;
            void setButtonType(ButtonType);

            std::shared_ptr<Core::IValueSubject<bool> > isChecked() const;
            void setChecked(bool);

            std::shared_ptr<Core::IValueSubject<Core::FileSystem::Path> > getIcon() const;
            std::shared_ptr<Core::IValueSubject<Style::MetricsRole> > getIconSizeRole() const;
            void setIcon(const Core::FileSystem::Path&);
            void setIconSizeRole(Style::MetricsRole);

            std::shared_ptr<Core::IValueSubject<std::string> > getText() const;
            void setText(const std::string&);

            std::shared_ptr<Core::IValueSubject<std::string> > getFontFace() const;
            std::shared_ptr<Core::IValueSubject<Style::MetricsRole> > getFontSizeRole() const;
            void setFontFace(const std::string &);
            void setFontSizeRole(Style::MetricsRole);

            const std::shared_ptr<Shortcut>& getShortcut() const;
            void setShortcut(const std::shared_ptr<Shortcut>&);

            std::shared_ptr<Core::IValueSubject<bool> > isEnabled() const;
            void setEnabled(bool);

            std::shared_ptr<Core::IValueSubject<HAlign> > getHAlign() const;
            std::shared_ptr<Core::IValueSubject<VAlign> > getVAlign() const;
            void setHAlign(HAlign);
            void setVAlign(VAlign);

            void setClickedCallback(const std::function<void(void)>&);
            void setCheckedCallback(const std::function<void(bool)>&);
            void doClickedCallback();
            void doCheckedCallback();

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class manages a group of actions.
        class ActionGroup : public std::enable_shared_from_this<ActionGroup>
        {
            DJV_NON_COPYABLE(ActionGroup);

        protected:
            ActionGroup();

        public:
            ~ActionGroup();

            static std::shared_ptr<ActionGroup> create(ButtonType);

            const std::vector<std::shared_ptr<Action> >& getActions() const;
            size_t getActionCount() const;
            int getActionIndex(const std::shared_ptr<Action>&) const;
            void addAction(const std::shared_ptr<Action>&);
            void removeAction(const std::shared_ptr<Action>&);
            void clearActions();

            ButtonType getButtonType() const;
            void setButtonType(ButtonType);

            void setChecked(int, bool = true);

            void setClickedCallback(const std::function<void(int)>&);
            void setCheckedCallback(const std::function<void(int, bool)>&);
            void setRadioCallback(const std::function<void(int)>&);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
