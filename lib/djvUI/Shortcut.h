// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/ValueObserver.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        class TextSystem;
    
    } // namespace Core

    namespace UI
    {
        //! This class provides a keyboard shortcut.
        class Shortcut : public std::enable_shared_from_this<Shortcut>
        {
            DJV_NON_COPYABLE(Shortcut);

        protected:
            void _init();
            Shortcut();

        public:
            virtual ~Shortcut();

            static std::shared_ptr<Shortcut> create();
            static std::shared_ptr<Shortcut> create(int key);
            static std::shared_ptr<Shortcut> create(int key, int modifiers);

            std::shared_ptr<Core::IValueSubject<int> > observeShortcutKey() const;
            void setShortcutKey(int key);

            std::shared_ptr<Core::IValueSubject<int> > observeShortcutModifiers() const;
            void setShortcutModifiers(int);

            void setCallback(const std::function<void(void)> &);
            void doCallback();

            static int getSystemModifier();
            static std::string getKeyString(int);
            static std::string getModifierString(int);
            static std::string getText(const std::shared_ptr<Shortcut>&, const std::shared_ptr<Core::TextSystem> &);
            static std::string getText(int key, int keyModifiers, const std::shared_ptr<Core::TextSystem> &);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
