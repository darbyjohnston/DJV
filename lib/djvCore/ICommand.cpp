// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/ICommand.h>

namespace djv
{
    namespace Core
    {
        ICommand::ICommand(const std::string& name, const std::shared_ptr<Context>& context) :
            _context(context),
            _name(name)
        {}

        ICommand::~ICommand()
        {}

        const std::weak_ptr<Context>& ICommand::getContext() const
        {
            return _context;
        }

        const std::string& ICommand::getName() const
        {
            return _name;
        }

    } // namespace Core
} // namespace djv
