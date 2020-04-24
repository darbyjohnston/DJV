// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/UndoStack.h>

#include <djvCore/ICommand.h>

#include <vector>

namespace djv
{
    namespace Core
    {
        struct UndoStack::Private
        {
            std::vector<std::shared_ptr<ICommand> > commands;
            int64_t currentIndex = -1;
            std::function<void()> callback;
        };

        UndoStack::UndoStack() :
            _p(new Private)
        {}

        UndoStack::~UndoStack()
        {}

        std::shared_ptr<UndoStack> UndoStack::create()
        {
            auto out = std::shared_ptr<UndoStack>(new UndoStack);
            return out;
        }

        const std::vector<std::shared_ptr<ICommand> > & UndoStack::getCommands() const
        {
            return _p->commands;
        }

        size_t UndoStack::getSize() const
        {
            return _p->commands.size();
        }

        int64_t UndoStack::getCurrentIndex() const
        {
            return _p->currentIndex;
        }

        void UndoStack::push(const std::shared_ptr<ICommand> & command)
        {
            DJV_PRIVATE_PTR();
            while (static_cast<int64_t>(p.commands.size()) - 1 > p.currentIndex)
            {
                p.commands.pop_back();
            }
            p.commands.push_back(command);
            ++p.currentIndex;
            command->exec();
            if (p.callback)
            {
                p.callback();
            }
        }

        void UndoStack::undo()
        {
            DJV_PRIVATE_PTR();
            if (p.commands.size() && p.currentIndex >= 0)
            {
                p.commands[p.currentIndex]->undo();
                --p.currentIndex;
                if (p.callback)
                {
                    p.callback();
                }
            }
        }

        void UndoStack::redo()
        {
            DJV_PRIVATE_PTR();
            if (p.commands.size() && p.currentIndex < static_cast<int64_t>(p.commands.size()) - 1)
            {
                ++p.currentIndex;
                p.commands[p.currentIndex]->exec();
                if (p.callback)
                {
                    p.callback();
                }
            }
        }

        void UndoStack::clear()
        {
            DJV_PRIVATE_PTR();
            if (p.commands.size())
            {
                p.commands.clear();
                p.currentIndex = -1;
                if (p.callback)
                {
                    p.callback();
                }
            }
        }

    } // namespace Core
} // namespace djv
