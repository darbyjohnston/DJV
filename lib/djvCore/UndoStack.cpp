//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCore/UndoStack.h>

#include <djvCore/Context.h>
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

        void UndoStack::_init(const std::shared_ptr<Context> & context)
        {}

        UndoStack::UndoStack() :
            _p(new Private)
        {}

        UndoStack::~UndoStack()
        {}

        std::shared_ptr<UndoStack> UndoStack::create(const std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<UndoStack>(new UndoStack);
            out->_init(context);
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
            while (static_cast<int64_t>(_p->commands.size()) - 1 > _p->currentIndex)
            {
                _p->commands.pop_back();
            }
            _p->commands.push_back(command);
            ++_p->currentIndex;
            command->exec();
            if (_p->callback)
            {
                _p->callback();
            }
        }

        void UndoStack::undo()
        {
            if (_p->commands.size() && _p->currentIndex >= 0)
            {
                _p->commands[_p->currentIndex]->undo();
                --_p->currentIndex;
                if (_p->callback)
                {
                    _p->callback();
                }
            }
        }

        void UndoStack::redo()
        {
            if (_p->commands.size() && _p->currentIndex < static_cast<int64_t>(_p->commands.size()) - 1)
            {
                ++_p->currentIndex;
                _p->commands[_p->currentIndex]->exec();
                if (_p->callback)
                {
                    _p->callback();
                }
            }
        }

        void UndoStack::clear()
        {
            if (_p->commands.size())
            {
                _p->commands.clear();
                _p->currentIndex = -1;
                if (_p->callback)
                {
                    _p->callback();
                }
            }
        }

    } // namespace Core
} // namespace djv
