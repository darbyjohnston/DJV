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

//! \file djvChoiceButton.h

#ifndef DJV_CHOICE_BUTTON_H
#define DJV_CHOICE_BUTTON_H

#include <djvAbstractToolButton.h>

#include <djvUtil.h>

struct djvChoiceButtonPrivate;

class QActionGroup;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvChoiceButton
//!
//! This class provides a multiple choice button. The choices are defined by
//! the QActionGroup associated with the button.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvChoiceButton : public djvAbstractToolButton
{
    Q_OBJECT
    
public:
    
    //! Constructor.
    
    explicit djvChoiceButton(QWidget * parent = 0);
    
    //! Constructor.
    
    explicit djvChoiceButton(QActionGroup *, QWidget * parent = 0);
    
    //! Destructor.
    
    virtual ~djvChoiceButton();
    
    //! Get the associated action group.

    QActionGroup * actionGroup() const;

    //! Get the current index.
    
    int currentIndex() const;
    
    virtual QSize sizeHint() const;

public slots:

    //! Set the action group. The ownership of the action group is not
    //! transferred to the button.

    void setActionGroup(QActionGroup *);

    //! Set the current index.

    void setCurrentIndex(int);
    
Q_SIGNALS:

    //! This signal is emitted when the current index is changed.
    
    void currentIndexChanged(int);
    
protected:

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private Q_SLOTS:

    void actionGroupCallback(QAction *);
    void clickedCallback();

private:
    
    DJV_PRIVATE_COPY(djvChoiceButton);
    
    djvChoiceButtonPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_CHOICE_BUTTON_H
