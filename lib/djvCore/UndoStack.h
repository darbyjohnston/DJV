// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ValueObserver.h>

#include <functional>
#include <memory>
#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Command
        {
            class ICommand;

            //! Undo stack.
            class UndoStack : public std::enable_shared_from_this<UndoStack>
            {
                DJV_NON_COPYABLE(UndoStack);

            protected:
                UndoStack();

            public:
                ~UndoStack();

                static std::shared_ptr<UndoStack> create();

                void push(const std::shared_ptr<ICommand>&);
                void clear();

                std::shared_ptr<Observer::IValueSubject<bool> > observeHasUndo() const;
                std::shared_ptr<Observer::IValueSubject<bool> > observeHasRedo() const;

                void undo();
                void redo();

            protected:
                DJV_PRIVATE();
            };

        } // namespace Command
    } // namespace Core
} // namespace djv
