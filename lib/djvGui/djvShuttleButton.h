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

//! \file djvShuttleButton.h

#ifndef DJV_SHUTTLE_BUTTON_H
#define DJV_SHUTTLE_BUTTON_H

#include <djvAbstractToolButton.h>

#include <djvUtil.h>

class  djvGuiContext;
struct djvShuttleButtonPrivate;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvShuttleButton
//!
//! This class provides a shuttle button.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvShuttleButton : public djvAbstractToolButton
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvShuttleButton(QWidget * parent = 0);

    //! Constructor.

    explicit djvShuttleButton(const QVector<QIcon> & icons, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvShuttleButton();
    
    //! Get the icons.
    
    const QVector<QIcon> & icons() const;

    //! Set the icons.
    
    void setIcons(const QVector<QIcon> &);

    //! Get the default shuttle icons.

    static const QVector<QIcon> & iconsDefault(djvGuiContext *);

    virtual QSize sizeHint() const;

Q_SIGNALS:

    //! This signal is emitted when the shuttle is pressed.

    void mousePressed(bool);

    //! This signal is emitted when the shuttle is changed.

    void valueChanged(int);
    
protected:

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private:

    void iconsUpdate();
    
    DJV_PRIVATE_COPY(djvShuttleButton);
    
    djvShuttleButtonPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_SHUTTLE_BUTTON_H

