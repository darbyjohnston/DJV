// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/UndoStackTest.h>

#include <djvCore/ICommand.h>
#include <djvCore/String.h>
#include <djvCore/UndoStack.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        UndoStackTest::UndoStackTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::UndoStackTest", tempPath, context)
        {}
        
        namespace
        {
            class AddCommand : public ICommand
            {
                DJV_NON_COPYABLE(AddCommand);

            protected:
                void _init(
                    int value,
                    const std::shared_ptr<std::vector<int> >& list)
                {
                    _value = value;
                    _list = list;
                }

                AddCommand()
                {}

            public:
                static std::shared_ptr<AddCommand> create(
                    int value,
                    const std::shared_ptr<std::vector<int> >& list)
                {
                    auto out = std::shared_ptr<AddCommand>(new AddCommand);
                    out->_init(value, list);
                    return out;
                }

                void exec() override
                {
                    _list->push_back(_value);
                }

                void undo() override
                {
                    _list->pop_back();
                }

            private:
                int _value = 0;
                std::shared_ptr<std::vector<int> > _list;
            };

        } // namespace

        void UndoStackTest::run()
        {
            std::shared_ptr<std::vector<int> > list(new std::vector<int>());
            auto undoStack = UndoStack::create();

            {
                DJV_ASSERT(!undoStack->observeHasUndo()->get());
                DJV_ASSERT(!undoStack->observeHasRedo()->get());

                undoStack->push(AddCommand::create(1, list));
                DJV_ASSERT(undoStack->observeHasUndo()->get());
                undoStack->push(AddCommand::create(2, list));
                undoStack->push(AddCommand::create(3, list));
                _printList(list);

                undoStack->undo();
                DJV_ASSERT(undoStack->observeHasRedo()->get());
                _printList(list);
                undoStack->undo();
                _printList(list);

                undoStack->redo();
                _printList(list);

                undoStack->undo();
                _printList(list);
                undoStack->undo();
                _printList(list);
                undoStack->undo();
                DJV_ASSERT(!undoStack->observeHasUndo()->get());
                _printList(list);

                undoStack->redo();
                DJV_ASSERT(undoStack->observeHasUndo()->get());
                _printList(list);
                undoStack->redo();
                _printList(list);
                undoStack->redo();
                _printList(list);
                undoStack->redo();
                DJV_ASSERT(!undoStack->observeHasRedo()->get());
                _printList(list);

                undoStack->clear();
                DJV_ASSERT(!undoStack->observeHasUndo()->get());
                DJV_ASSERT(!undoStack->observeHasRedo()->get());
                _printList(list);

                undoStack->push(AddCommand::create(1, list));
                undoStack->undo();
                undoStack->push(AddCommand::create(2, list));
            }
        }

        void UndoStackTest::_printList(const std::shared_ptr<std::vector<int> >& value)
        {
            std::vector<std::string> s;
            for (const auto& i : *value)
            {
                std::stringstream ss;
                ss << i;
                s.push_back(ss.str());
            }
            _print(String::join(s, ", "));
        }
        
    } // namespace CoreTest
} // namespace djv

