// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <memory>
#include <string>

namespace djv
{
    namespace Core
    {
        class Context;

        //! This class provides the interface for commands.
        class ICommand
        {
        public:
            ICommand(const std::string & name, const std::shared_ptr<Context>&);
            virtual ~ICommand() = 0;

            const std::weak_ptr<Context>& getContext() const;
            const std::string& getName() const;

            virtual void exec() = 0;
            virtual void undo() = 0;

        private:
            std::weak_ptr<Context> _context;
            std::string _name;
        };

    } // namespace Core
} // namespace djv
