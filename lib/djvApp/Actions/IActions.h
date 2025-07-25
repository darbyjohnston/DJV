// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/SettingsModel.h>

#include <feather-tk/ui/Action.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Base class for action groups.
        class IActions : public std::enable_shared_from_this<IActions>
        {
            FEATHER_TK_NON_COPYABLE(IActions);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::string& name);

            IActions();

        public:
            ~IActions();

            const std::map<std::string, std::shared_ptr<feather_tk::Action> >& getActions() const;

        protected:
            void _shortcutsUpdate(const ShortcutsSettings&);

            std::string _name;
            std::map<std::string, std::shared_ptr<feather_tk::Action> > _actions;
            std::map<std::string, std::string> _tooltips;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
