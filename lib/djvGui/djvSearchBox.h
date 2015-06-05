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

//! \file djvSearchBox.h

#ifndef DJV_SEARCH_BOX_H
#define DJV_SEARCH_BOX_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QWidget>

class  djvGuiContext;
struct djvSearchBoxPrivate;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvSearchBox
//!
//! This class provides a search box widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvSearchBox : public QWidget
{
    Q_OBJECT
    
    //! This property holds the test.
    
    Q_PROPERTY(
        QString text
        READ    text
        WRITE   setText
        NOTIFY  textChanged)
    
public:

    //! Constructor.

    explicit djvSearchBox(djvGuiContext *, QWidget * parent = 0);
    
    //! Destructor.
    
    virtual ~djvSearchBox();

    //! Get the text.

    const QString & text() const;
    
public Q_SLOTS:

    //! Set the text.

    void setText(const QString &);

Q_SIGNALS:

    //! This signal is emitted when the text is changed.

    void textChanged(const QString &);
    
private Q_SLOTS:

    void textCallback(const QString &);
    void resetCallback();

private:

    DJV_PRIVATE_COPY(djvSearchBox);
    
    djvSearchBoxPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_SEARCH_BOX_H

