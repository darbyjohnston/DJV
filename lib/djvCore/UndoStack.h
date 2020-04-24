// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <functional>
#include <memory>
#include <vector>

namespace djv
{
    namespace Core
    {
        class ICommand;

        //! This class provides an undo stack.
        class UndoStack : public std::enable_shared_from_this<UndoStack>
        {
            DJV_NON_COPYABLE(UndoStack);

        protected:
            UndoStack();

        public:
            ~UndoStack();

            static std::shared_ptr<UndoStack> create();

            const std::vector<std::shared_ptr<ICommand> > & getCommands() const;
            size_t getSize() const;
            int64_t getCurrentIndex() const;

            void push(const std::shared_ptr<ICommand> &);
            void undo();
            void redo();
            void clear();

            void setCallback(const std::function<void()> &);

        protected:
            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv
