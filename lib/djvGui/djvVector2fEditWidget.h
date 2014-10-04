//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvVector2fEditWidget.h

#ifndef DJV_VECTOR2F_EDIT_WIDGET_H
#define DJV_VECTOR2F_EDIT_WIDGET_H

#include <djvGuiExport.h>

#include <djvUtil.h>
#include <djvVector.h>

#include <QWidget>

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvVector2fEditWidget
//!
//! This class provides a two-dimensional floating point vector widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvVector2fEditWidget : public QWidget
{
    Q_OBJECT
    
    //! This property holds the value.
    
    Q_PROPERTY(
        djvVector2f value
        READ        value
        WRITE       setValue
        NOTIFY      valueChanged)
    
public:

    //! Constructor.

    explicit djvVector2fEditWidget(QWidget * parent = 0);

    //! Destructor.

    virtual ~djvVector2fEditWidget();

    //! Get the value.

    const djvVector2f & value() const;

public Q_SLOTS:

    //! Set the value.

    void setValue(const djvVector2f &);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(const djvVector2f &);

private Q_SLOTS:

    void widgetCallback(double);
    void widget2Callback(double);

private:

    void widgetUpdate();
    
    DJV_PRIVATE_COPY(djvVector2fEditWidget);
    DJV_PRIVATE_IMPLEMENTATION();
};

//@} // djvGuiWidget

#endif // DJV_VECTOR2F_EDIT_WIDGET_H

