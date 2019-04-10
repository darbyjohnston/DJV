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

#include <djvViewLib/AnnotateTool.h>

#include <djvViewLib/AnnotateActions.h>
#include <djvViewLib/AnnotateData.h>
#include <djvViewLib/AnnotateGroup.h>
#include <djvViewLib/AnnotateModel.h>
#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/PlaybackGroup.h>
#include <djvViewLib/AnnotateWidget.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Color.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QTabWidget>
#include <QTreeView>
#include <QMenu>
#include <QPointer>
#include <QSpinBox>
#include <QTextEdit>

#undef DELETE

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateTool::Private
        {};

        AnnotateTool::AnnotateTool(
            const QPointer<AnnotateActions> & annotateActions,
            const QPointer<AnnotateGroup> & annotateGroup,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private)
        {
            // Create the widgets.
            auto tabWidget = new QTabWidget;
            auto editWidget = new AnnotateEditWidget(annotateActions, annotateGroup, session, context);
            auto summaryWidget = new AnnotateSummaryWidget(annotateGroup, session, context);
            auto exportWidget = new AnnotateExportWidget(annotateGroup, session, context);

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(tabWidget);
            tabWidget->addTab(editWidget, qApp->translate("djv::ViewLib::AnnotateTool", "Edit"));
            tabWidget->addTab(summaryWidget, qApp->translate("djv::ViewLib::AnnotateTool", "Summary"));
            tabWidget->addTab(exportWidget, qApp->translate("djv::ViewLib::AnnotateTool", "Export"));

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateTool", "Annotate"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }

        AnnotateTool::~AnnotateTool()
        {}

    } // namespace ViewLib
} // namespace djv
