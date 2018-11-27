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

#pragma once

#include <Core/Util.h>

#include <QObject>
#include <QAbstractListModel>

namespace djv
{
    namespace Core
    {
        class Context;
        class ICommand;

        class UndoStack : public QObject
        {
            Q_OBJECT

        public:
            UndoStack(const QPointer<Context> &);
            ~UndoStack() override;

            const std::vector<std::shared_ptr<ICommand> > & getCommands() const;
            size_t getSize() const;
            int64_t getCurrentIndex() const;

            void push(const std::shared_ptr<ICommand> &);
            void undo();
            void redo();
            void clear();

        Q_SIGNALS:
            void stackChanged();

        protected:
            DJV_PRIVATE();
        };

        class UndoStackModel : public QAbstractListModel
        {
            Q_OBJECT

        public:
            UndoStackModel(const QPointer<UndoStack> &);
            ~UndoStackModel() override;

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv
