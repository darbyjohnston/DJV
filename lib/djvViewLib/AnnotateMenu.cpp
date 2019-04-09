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

#include <djvViewLib/AnnotateMenu.h>

#include <djvViewLib/AnnotateActions.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateMenu::Private
        {
        };

        AnnotateMenu::AnnotateMenu(
            const QPointer<AbstractActions> & actions,
            QWidget * parent) :
            AbstractMenu(actions, parent),
            _p(new Private)
        {
            addAction(actions->action(AnnotateActions::SHOW));
            addSeparator();
            addActions(actions->group(AnnotateActions::PRIMITIVE_GROUP)->actions());
            addAction(actions->action(AnnotateActions::COLOR));
            addAction(actions->action(AnnotateActions::LINE_WIDTH_INC));
            addAction(actions->action(AnnotateActions::LINE_WIDTH_DEC));
            //addAction(actions->action(AnnotateActions::UNDO));
            //addAction(actions->action(AnnotateActions::REDO));
            addAction(actions->action(AnnotateActions::CLEAR));
            addSeparator();
            addAction(actions->action(AnnotateActions::NEW));
            addAction(actions->action(AnnotateActions::DELETE));
            addAction(actions->action(AnnotateActions::DELETE_ALL));
            addAction(actions->action(AnnotateActions::NEXT));
            addAction(actions->action(AnnotateActions::PREV));
            addSeparator();
            addAction(actions->action(AnnotateActions::EXPORT));

            setTitle(qApp->translate("djv::ViewLib::AnnotateMenu", "&Annotate"));
        }

        AnnotateMenu::~AnnotateMenu()
        {}

    } // namespace ViewLib
} // namespace djv
