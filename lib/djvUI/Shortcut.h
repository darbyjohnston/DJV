// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace UI
    {
        struct ShortcutData;

        //! This class provides a keyboard shortcut.
        class Shortcut : public std::enable_shared_from_this<Shortcut>
        {
            DJV_NON_COPYABLE(Shortcut);

        protected:
            void _init();
            Shortcut();

        public:
            ~Shortcut();

            static std::shared_ptr<Shortcut> create();
            static std::shared_ptr<Shortcut> create(const ShortcutData&);
            static std::shared_ptr<Shortcut> create(int key);
            static std::shared_ptr<Shortcut> create(int key, int modifiers);

            std::shared_ptr<Core::Observer::IValueSubject<ShortcutData> > observeShortcut() const;

            void setShortcut(const ShortcutData&);

            void setCallback(const std::function<void(void)>&);
            void doCallback();

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
