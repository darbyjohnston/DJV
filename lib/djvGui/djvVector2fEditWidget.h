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

//! \file djvVector2fEditWidget.h

#ifndef DJV_VECTOR2F_EDIT_WIDGET_H
#define DJV_VECTOR2F_EDIT_WIDGET_H

#include <djvGuiExport.h>

#include <djvUtil.h>
#include <djvVector.h>

#include <QWidget>

struct djvVector2fEditWidgetPrivate;

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
    
    //! This property holds the minimum value.
    
    Q_PROPERTY(
        djvVector2f min
        READ        min
        WRITE       setMin
        NOTIFY      minChanged)
    
    //! This property holds the maximum value.
    
    Q_PROPERTY(
        djvVector2f max
        READ        max
        WRITE       setMax
        NOTIFY      maxChanged)
    
public:

    //! Constructor.

    explicit djvVector2fEditWidget(QWidget * parent = 0);

    //! Destructor.

    virtual ~djvVector2fEditWidget();

    //! Get the value.

    djvVector2f value() const;

    //! Get the minimum value.

    djvVector2f min() const;

    //! Get the maximum value.

    djvVector2f max() const;

public Q_SLOTS:

    //! Set the value.

    void setValue(const djvVector2f &);

    //! Set the minimum value.

    void setMin(const djvVector2f &);

    //! Set the maximum value.

    void setMax(const djvVector2f &);

    //! Set the value range.

    void setRange(const djvVector2f & min, const djvVector2f & max);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(const djvVector2f &);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(const djvVector2f &);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(const djvVector2f &);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(const djvVector2f &, const djvVector2f &);

private Q_SLOTS:

    void valueCallback();
    void rangeCallback();

private:

    DJV_PRIVATE_COPY(djvVector2fEditWidget);
    
    djvVector2fEditWidgetPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_VECTOR2F_EDIT_WIDGET_H

