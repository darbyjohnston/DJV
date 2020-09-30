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
        namespace Command
        {
            struct UndoStack::Private
            {
                std::vector<std::shared_ptr<ICommand> > commands;
                int64_t currentIndex = -1;
                std::shared_ptr<Observer::ValueSubject<bool> > hasUndo;
                std::shared_ptr<Observer::ValueSubject<bool> > hasRedo;
            };

            UndoStack::UndoStack() :
                _p(new Private)
            {
                DJV_PRIVATE_PTR();
                p.hasUndo = Observer::ValueSubject<bool>::create(false);
                p.hasRedo = Observer::ValueSubject<bool>::create(false);
            }

            UndoStack::~UndoStack()
            {}

            std::shared_ptr<UndoStack> UndoStack::create()
            {
                auto out = std::shared_ptr<UndoStack>(new UndoStack);
                return out;
            }

            void UndoStack::push(const std::shared_ptr<ICommand>& command)
            {
                DJV_PRIVATE_PTR();
                while (static_cast<int64_t>(p.commands.size()) - 1 > p.currentIndex)
                {
                    p.commands.pop_back();
                }
                p.commands.push_back(command);
                p.currentIndex = p.currentIndex + 1;
                command->exec();
                p.hasUndo->setIfChanged(true);
            }

            std::shared_ptr<Observer::IValueSubject<bool> > UndoStack::observeHasUndo() const
            {
                return _p->hasUndo;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > UndoStack::observeHasRedo() const
            {
                return _p->hasRedo;
            }

            void UndoStack::undo()
            {
                DJV_PRIVATE_PTR();
                if (p.commands.size() && p.currentIndex >= 0)
                {
                    p.commands[p.currentIndex]->undo();
                    p.currentIndex = p.currentIndex - 1;
                    p.hasUndo->setIfChanged(p.currentIndex >= 0);
                    p.hasRedo->setIfChanged(p.currentIndex < static_cast<int64_t>(p.commands.size()) - 1);
                }
            }

            void UndoStack::redo()
            {
                DJV_PRIVATE_PTR();
                if (p.commands.size() && p.currentIndex < static_cast<int64_t>(p.commands.size()) - 1)
                {
                    const int64_t index = p.currentIndex + 1;
                    p.commands[index]->exec();
                    p.currentIndex = index;
                    p.hasUndo->setIfChanged(p.currentIndex >= 0);
                    p.hasRedo->setIfChanged(p.currentIndex < static_cast<int64_t>(p.commands.size()) - 1);
                }
            }

            void UndoStack::clear()
            {
                DJV_PRIVATE_PTR();
                if (!p.commands.empty())
                {
                    p.commands.clear();
                    p.currentIndex = -1;
                    p.hasUndo->setIfChanged(false);
                    p.hasRedo->setIfChanged(false);
                }
            }

        } // namespace Command
    } // namespace Core
} // namespace djv
