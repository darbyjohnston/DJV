//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/ViewLib.h>

#include <djvCore/Util.h>

#include <QMap>
#include <QObject>

#include <memory>

class QAction;
class QActionGroup;

namespace djv
{
    namespace ViewLib
    {
        class ViewContext;

        //! This class provides the base functionality for groups of actions.
        class AbstractActions : public QObject
        {
            Q_OBJECT

        public:
            explicit AbstractActions(const QPointer<ViewContext> &, QObject * parent = nullptr);
            virtual ~AbstractActions() = 0;

            //! Get the list of actions.
            const QList<QAction *> actions() const;

            //! Get an action by name.
            QAction * action(int) const;

            //! Get the list of action groups.
            const QList<QActionGroup *> groups() const;

            //! Get an action group by index.
            QActionGroup * group(int) const;

            //! Get the context.
            const QPointer<ViewContext> & context() const;

        Q_SIGNALS:
            //! This signal is emitted when actions or action groups are changed.
            void changed();

        protected:
            QMap<int, QAction *>      _actions;
            QMap<int, QActionGroup *> _groups;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
