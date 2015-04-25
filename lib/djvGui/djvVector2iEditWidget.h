
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

//! \file djvVector2iEditWidget.h

#ifndef DJV_VECTOR2I_EDIT_WIDGET_H
#define DJV_VECTOR2I_EDIT_WIDGET_H

#include <djvGuiExport.h>

#include <djvUtil.h>
#include <djvVector.h>

#include <QWidget>

struct djvVector2iEditWidgetPrivate;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvVector2iEditWidget
//!
//! This class provides a two-dimensional integer vector widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvVector2iEditWidget : public QWidget
{
    Q_OBJECT
    
    //! This property holds the value.
    
    Q_PROPERTY(
        djvVector2i value
        READ        value
        WRITE       setValue
        NOTIFY      valueChanged)
    
    //! This property holds the minimum value.
    
    Q_PROPERTY(
        djvVector2i min
        READ        min
        WRITE       setMin
        NOTIFY      minChanged)
    
    //! This property holds the maximum value.
    
    Q_PROPERTY(
        djvVector2i max
        READ        max
        WRITE       setMax
        NOTIFY      maxChanged)
    
public:

    //! Constructor.

    explicit djvVector2iEditWidget(QWidget * parent = 0);

    //! Destructor.

    virtual ~djvVector2iEditWidget();

    //! Get the value.

    djvVector2i value() const;

    //! Get the minimum value.

    djvVector2i min() const;

    //! Get the maximum value.

    djvVector2i max() const;
    
public Q_SLOTS:

    //! Set the value.

    void setValue(const djvVector2i &);

    //! Set the minimum value.

    void setMin(const djvVector2i &);

    //! Set the maximum value.

    void setMax(const djvVector2i &);

    //! Set the value range.

    void setRange(const djvVector2i & min, const djvVector2i & max);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(const djvVector2i &);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(const djvVector2i &);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(const djvVector2i &);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(const djvVector2i &, const djvVector2i &);

private Q_SLOTS:

    void valueCallback();
    void rangeCallback();

private:

    DJV_PRIVATE_COPY(djvVector2iEditWidget);
    
    djvVector2iEditWidgetPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_VECTOR2I_EDIT_WIDGET_H

