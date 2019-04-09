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

#include <djvViewLib/AnnotateExportDialog.h>

#include <djvViewLib/AnnotatePrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/FileEdit.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QTextEdit>
#include <QVBoxLayout>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct AnnotateExportDialog::Private
        {
            Private(const QPointer<ViewContext> & context) :
                scriptFileInfo(context->annotatePrefs()->exportScript()),
                scriptOptions(context->annotatePrefs()->exportScriptOptions())
            {}

            FileInfo outputFileInfo;
            FileInfo scriptFileInfo;
            QString scriptOptions;
            QPointer< UI::FileEdit> outputFileEdit;
            QPointer< UI::FileEdit> scriptFileEdit;
            QPointer<QLineEdit> scriptOptionsLineEdit;
            QPointer<QDialogButtonBox> buttonBox;
        };

        AnnotateExportDialog::AnnotateExportDialog(const QPointer<ViewContext> & context) :
            _p(new Private(context))
        {
            auto summaryTextEdit = new QTextEdit;
            auto summaryGroupBox = new QGroupBox(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Summary"));
            auto vLayout = new QVBoxLayout;
            vLayout->addWidget(summaryTextEdit);
            summaryGroupBox->setLayout(vLayout);

            _p->outputFileEdit = new UI::FileEdit(context.data());
            auto outputFileGroupBox = new QGroupBox(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Output File"));
            vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->outputFileEdit);
            outputFileGroupBox->setLayout(vLayout);

            _p->scriptFileEdit = new UI::FileEdit(context.data());
            _p->scriptOptionsLineEdit = new QLineEdit;
            auto scriptGroupBox = new QGroupBox(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Script"));
            vLayout = new QVBoxLayout;
            vLayout->addWidget(_p->scriptFileEdit);
            auto formLayout = new QFormLayout;
            formLayout->addRow(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Options:"), _p->scriptOptionsLineEdit);
            vLayout->addLayout(formLayout);
            scriptGroupBox->setLayout(vLayout);

            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

            auto layout = new QVBoxLayout(this);
            layout->addWidget(summaryGroupBox);
            layout->addWidget(outputFileGroupBox);
            layout->addWidget(scriptGroupBox);
            layout->addStretch(1);
            layout->addWidget(_p->buttonBox);

            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateExportDialog", "Export Annotations"));
            setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
            widgetUpdate();

            connect(
                _p->scriptFileEdit,
                &UI::FileEdit::fileInfoChanged,
                [this, context](const FileInfo & value)
            {
                _p->scriptFileInfo = value;
                context->annotatePrefs()->setExportScript(value);
            });

            connect(
                _p->scriptOptionsLineEdit,
                &QLineEdit::editingFinished,
                [this, context]
            {
                const auto & text = _p->scriptOptionsLineEdit->text();
                _p->scriptOptions = text;
                context->annotatePrefs()->setExportScriptOptions(text);
            });

            connect(_p->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportScriptChanged,
                [this](const FileInfo & value)
            {
                _p->scriptFileInfo = value;
                widgetUpdate();
            });
            connect(
                context->annotatePrefs(),
                &AnnotatePrefs::exportScriptOptionsChanged,
                [this](const QString & value)
            {
                _p->scriptOptions = value;
                widgetUpdate();
            });
        }

        AnnotateExportDialog::~AnnotateExportDialog()
        {}

        void AnnotateExportDialog::widgetUpdate()
        {
            _p->scriptFileEdit->setFileInfo(_p->scriptFileInfo);
            _p->scriptOptionsLineEdit->setText(_p->scriptOptions);
        }

    } // namespace ViewLib
} // namespace djv
