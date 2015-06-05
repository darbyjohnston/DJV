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

//! \file djvFloatEditSlider.h

#ifndef DJV_FLOAT_EDIT_SLIDER_H
#define DJV_FLOAT_EDIT_SLIDER_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QWidget>

struct djvFloatEditSliderPrivate;
class  djvFloatObject;
class  djvGuiContext;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvFloatEditSlider
//!
//! This class provides an floating point slider and edit widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFloatEditSlider : public QWidget
{
    Q_OBJECT
    
    //! This property holds the value.
    
    Q_PROPERTY(
        double value
        READ   value
        WRITE  setValue
        NOTIFY valueChanged)
    
    //! This property holds the default value.
    
    Q_PROPERTY(
        double defaultValue
        READ   defaultValue
        WRITE  setDefaultValue
        NOTIFY defaultValueChanged)
    
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

    explicit djvFloatEditSlider(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvFloatEditSlider();

    //! Get the value.

    double value() const;

    //! Get the default value.

    double defaultValue() const;

    //! Get whether a reset to default control is shown.

    bool hasResetToDefault() const;

    //! Get the minimum value.

    double min() const;

    //! Get the maximum value.

    double max() const;

    //! Get the small increment.

    double smallInc() const;

    //! Get the large increment.

    double largeInc() const;

    //! Get the edit floating-point object.

    djvFloatObject * editObject() const;

    //! Get the slider floating-point object.

    djvFloatObject * sliderObject() const;
    
public Q_SLOTS:

    //! Set the value.

    void setValue(double);

    //! Set the default value.

    void setDefaultValue(double);

    //! Set whether a reset to default control is shown.

    void setResetToDefault(bool);

    //! Set the minimum value.

    void setMin(double);

    //! Set the maximum value.

    void setMax(double);

    //! Set the value range.

    void setRange(double min, double max);

    //! Set the value increment.

    void setInc(double smallInc, double largeInc);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(double);

    //! This signal is emitted when the default value is changed.

    void defaultValueChanged(double);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(double);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(double);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(double, double);

private Q_SLOTS:

    void valueCallback();
    void sliderCallback(double);
    void defaultCallback();
    
    void widgetUpdate();

private:
    
    DJV_PRIVATE_COPY(djvFloatEditSlider);
    
    djvFloatEditSliderPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_FLOAT_EDIT_SLIDER_H

