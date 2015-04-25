//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvMultiChoiceDialog.h

#ifndef DJV_MULTI_CHOICE_DIALOG_H
#define DJV_MULTI_CHOICE_DIALOG_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QDialog>

struct djvMultiChoiceDialogPrivate;

//! \addtogroup djvGuiDialog
//@{

//------------------------------------------------------------------------------
//! \class djvMultiChoiceDialog
//!
//! This class provides a multiple choice dialog.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvMultiChoiceDialog : public QDialog
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvMultiChoiceDialog(
        const QString &       label   = QString(),
        const QStringList &   choices = QStringList(),
        const QVector<bool> & values  = QVector<bool>(),
        QWidget *             parent  = 0);

    //! Destructor.
    
    virtual ~djvMultiChoiceDialog();
    
    //! Get the label.

    const QString & label() const;

    //! Set the label.

    void setLabel(const QString &);
    
    //! Get the choices.
    
    const QStringList & choices() const;

    //! Set the choices.

    void setChoices(const QStringList &);

    //! Get the choice values.

    const QVector<bool> & values() const;

    //! Get the choice indices.

    QVector<int> indices() const;
    
    //! Set the choice values.

    void setValues(const QVector<bool> &);

private Q_SLOTS:

    void buttonCallback();

private:

    void valuesUpdate();
    void widgetUpdate();

    DJV_PRIVATE_COPY(djvMultiChoiceDialog);
    
    djvMultiChoiceDialogPrivate * _p;
};

//@} // djvGuiDialog

#endif // DJV_MULTI_CHOICE_DIALOG_H

