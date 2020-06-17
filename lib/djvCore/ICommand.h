// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        //! This class provides the interface for commands.
        class ICommand : public std::enable_shared_from_this<ICommand>
        {
        public:
            virtual ~ICommand() = 0;

            virtual void exec() = 0;
            virtual void undo() = 0;
        };

    } // namespace Core
} // namespace djv
