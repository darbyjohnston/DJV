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

//! \file djvFloatSlider.h

#ifndef DJV_FLOAT_SLIDER_H
#define DJV_FLOAT_SLIDER_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QWidget>

class  djvFloatObject;
struct djvFloatSliderPrivate;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvFloatSlider
//!
//! This class provides a floating-point slider widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFloatSlider : public QWidget
{
    Q_OBJECT
    
    //! This property holds the value.
    
    Q_PROPERTY(
        double value
        READ   value
        WRITE  setValue
        NOTIFY valueChanged)
    
    //! This property holds the minimum value.
    
    Q_PROPERTY(
        double min
        READ   min
        WRITE  setMin
        NOTIFY minChanged)
    
    //! This property holds the maximum value.
    
    Q_PROPERTY(
        double max
        READ   max
        WRITE  setMax
        NOTIFY maxChanged)
    
public:

    //! Constructor.

    explicit djvFloatSlider(QWidget * parent = 0);

    //! Destructor.
    
    virtual ~djvFloatSlider();

    //! Get the value.

    double value() const;

    //! Get the minimum value.

    double min() const;

    //! Get the maximum value.

    double max() const;

    //! Get the floating-point object.

    djvFloatObject * object() const;
    
public Q_SLOTS:

    //! Set the value.

    void setValue(double);

    //! Set the minimum value.

    void setMin(double);

    //! Set the maximum value.

    void setMax(double);

    //! Set the value range.

    void setRange(double min, double max);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(double);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(double);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(double);

    //! This signal is emitted when the range is changed.

    void rangeChanged(double, double);
    
protected:

    virtual void keyPressEvent(QKeyEvent *);

private Q_SLOTS:

    void valueCallback();
    void rangeCallback();
    void sliderCallback(int);

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvFloatSlider);
    
    djvFloatSliderPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_FLOAT_SLIDER_H

