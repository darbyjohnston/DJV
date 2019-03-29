//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewLib/AbstractActions.h>

#include <QAction>
#include <QActionGroup>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct AbstractActions::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            QPointer<ViewContext> context;
        };

        AbstractActions::AbstractActions(
            const QPointer<ViewContext> & context,
            QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {}

        AbstractActions::~AbstractActions()
        {}

        const QList<QAction *> AbstractActions::actions() const
        {
            return _actions.values();
        }

        QAction * AbstractActions::action(int index) const
        {
            return _actions.contains(index) ? _actions[index] : 0;
        }

        const QList<QActionGroup *> AbstractActions::groups() const
        {
            return _groups.values();
        }

        QActionGroup * AbstractActions::group(int index) const
        {
            return _groups.contains(index) ? _groups[index] : 0;
        }

        const QPointer<ViewContext> & AbstractActions::context() const
        {
            return _p->context;
        }

    } // namespace ViewLib
} // namespace djv
