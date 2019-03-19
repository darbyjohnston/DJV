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

#include <djvUI/Style.h>

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
            void doChecked();

            std::shared_ptr<Core::IValueSubject<std::string> > observeIcon() const;
            void setIcon(const std::string &);

            std::shared_ptr<Core::IValueSubject<std::string> > observeTitle() const;
            void setTitle(const std::string &);

            std::shared_ptr<Core::IValueSubject<std::string> > observeText() const;
            void setText(const std::string &);

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
            void setTooltip(const std::string &);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
