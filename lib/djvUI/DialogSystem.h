// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/UI.h>

#include <djvCore/ISystem.h>

#include <functional>

namespace djv
{
    namespace UI
    {
        //! This class provides standard dialogs.
        class DialogSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(DialogSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            DialogSystem();

        public:
            virtual ~DialogSystem();

            static std::shared_ptr<DialogSystem> create(const std::shared_ptr<Core::Context>&);

            //! Show a message dialog.
            void message(
                const std::string& title,
                const std::string& text,
                const std::string& closeText);

            //! Show a confirmation dialog.
            void confirmation(
                const std::string& title,
                const std::string& text,
                const std::string& acceptText,
                const std::string& cancelText,
                const std::function<void(bool)>& callback);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

