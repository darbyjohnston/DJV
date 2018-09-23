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

#include <djvUI/FileEdit.h>

#include <djvUI/UIContext.h>
#include <djvUI/FileBrowser.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>

namespace djv
{
    namespace UI
    {
        struct FileEdit::Private
        {
            Private(UIContext * context) :
                context(context)
            {}

            Core::FileInfo fileInfo;
            QPointer<QLineEdit> edit;
            UIContext * context = nullptr;
        };

        FileEdit::FileEdit(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private(context))
        {
            // Create the widgets.
            _p->edit = new QLineEdit;

            QPushButton * button = new QPushButton(
                qApp->translate("djv::UI::FileEdit", "&Browse"));

            // Layout the widgets.
            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->edit, 1);
            layout->addWidget(button);

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(_p->edit, SIGNAL(editingFinished()), SLOT(editCallback()));
            connect(button, SIGNAL(clicked()), SLOT(buttonCallback()));
        }

        FileEdit::~FileEdit()
        {}
        
        const Core::FileInfo & FileEdit::fileInfo() const
        {
            return _p->fileInfo;
        }

        void FileEdit::setFileInfo(const Core::FileInfo & file)
        {
            if (file == _p->fileInfo)
                return;
            _p->fileInfo = file;
            widgetUpdate();
            Q_EMIT fileInfoChanged(_p->fileInfo);
        }

        void FileEdit::editCallback()
        {
            setFileInfo(_p->edit->text());
        }

        void FileEdit::buttonCallback()
        {
            FileBrowser * fileBrowser = _p->context->fileBrowser();
            if (QDialog::Accepted == fileBrowser->exec())
            {
                setFileInfo(fileBrowser->fileInfo());
            }
        }

        void FileEdit::widgetUpdate()
        {
            _p->edit->setText(_p->fileInfo);
        }

    } // namespace UI
} // namespace djv
