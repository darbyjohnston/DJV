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

//! \file djvIntEditSlider.h

#ifndef DJV_INT_EDIT_SLIDER_H
#define DJV_INT_EDIT_SLIDER_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QWidget>

class  djvGuiContext;
struct djvIntEditSliderPrivate;
class  djvIntObject;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvIntEditSlider
//!
//! This class provides an integer slider and edit widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvIntEditSlider : public QWidget
{
    Q_OBJECT
    
    //! This property holds the value.
    
    Q_PROPERTY(
        int    value
        READ   value
        WRITE  setValue
        NOTIFY valueChanged)
    
    //! This property holds the default value.
    
    Q_PROPERTY(
        int    defaultValue
        READ   defaultValue
        WRITE  setDefaultValue
        NOTIFY defaultValueChanged)
    
    //! This property holds the minimum value.
    
    Q_PROPERTY(
        int    min
        READ   min
        WRITE  setMin
        NOTIFY minChanged)
    
    //! This property holds the maximum value.
    
    Q_PROPERTY(
        int    max
        READ   max
        WRITE  setMax
        NOTIFY maxChanged)
    
public:

    //! Constructor.

    explicit djvIntEditSlider(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvIntEditSlider();

    //! Get the value.

    int value() const;

    //! Get the default value.

    int defaultValue() const;

    //! Get whether a reset to default control is shown.

    bool hasResetToDefault() const;

    //! Get the minimum value.

    int min() const;

    //! Get the maximum value.

    int max() const;

    //! Get the edit integer object.

    djvIntObject * editObject() const;

    //! Get the slider integer object.

    djvIntObject * sliderObject() const;

public Q_SLOTS:

    //! Set the value.

    void setValue(int);

    //! Set the default value.

    void setDefaultValue(int);

    //! Set whether a reset to default control is shown.

    void setResetToDefault(bool);

    //! Set the minimum value.

    void setMin(int);

    //! Set the maximum value.

    void setMax(int);

    //! Set the value range.

    void setRange(int min, int max);
    
Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(int);

    //! This signal is emitted when the default value is changed.

    void defaultValueChanged(int);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(int);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(int);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(int, int);

private Q_SLOTS:

    void valueCallback();
    void sliderCallback(int);
    void defaultCallback();

    void widgetUpdate();
    
private:

    DJV_PRIVATE_COPY(djvIntEditSlider);
    
    djvIntEditSliderPrivate * _p;
};

//@} // djvGuiWidget

#endif // DJV_INT_EDIT_SLIDER_H

