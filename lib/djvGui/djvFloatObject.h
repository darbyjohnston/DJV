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

//! \file djvFloatObject.h

#ifndef DJV_FLOAT_OBJECT_H
#define DJV_FLOAT_OBJECT_H

#include <djvGuiExport.h>

#include <QObject>

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvFloatObject
//!
//! This class provides the base functionality for floating point widgets.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvFloatObject : public QObject
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
    
    //! This property holds whether the default value is valid.
    
    Q_PROPERTY(
        bool   defaultValid
        READ   isDefaultValid
        NOTIFY defaultValidChanged)
    
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
    
    //! This property holds the small increment.
    
    Q_PROPERTY(
        double smallInc
        READ   smallInc
        WRITE  setSmallInc
        NOTIFY smallIncChanged)
    
    //! This property holds the large increment.
    
    Q_PROPERTY(
        double largeInc
        READ   largeInc
        WRITE  setLargeInc
        NOTIFY largeIncChanged)
    
    //! This property holds whether clamping is enabled.
    
    Q_PROPERTY(
        bool  clamp
        READ  hasClamp
        WRITE setClamp)

    //! This property holds the size string.
    
    Q_PROPERTY(
        QString sizeString
        READ    sizeString
        WRITE   setSizeString
        NOTIFY  sizeStringChanged)

public:

    //! The minimum floating point value.

    static const double floatMin;

    //! The maximum floating point value.

    static const double floatMax;

    //! Constructor.

    explicit djvFloatObject(QObject * parent = 0);

    //! Destructor.

    virtual ~djvFloatObject();

    //! Get the value.

    double value() const;

    //! Get the default value.

    double defaultValue() const;

    //! Get whether the default value is valid.

    bool isDefaultValid() const;

    //! Get the minimum value.

    double min() const;

    //! Get the maximum value.

    double max() const;

    //! Get the small increment.

    double smallInc() const;

    //! Get the large increment.

    double largeInc() const;

    //! Get whether clamping is enabled.

    bool hasClamp() const;

    //! Get the size string.

    const QString & sizeString() const;

public Q_SLOTS:

    //! Set the value.

    void setValue(double);

    //! Set the default value.

    void setDefaultValue(double);

    //! Set the minimum value.

    void setMin(double);

    //! Set the maximum value.

    void setMax(double);

    //! Set the value range.

    void setRange(double min, double max);

    //! Set the value to the minimum.

    void setToMin();

    //! Set the value to the maximum.

    void setToMax();

    //! Set the small increment.

    void setSmallInc(double);

    //! Set the large increment.

    void setLargeInc(double);

    //! Set the value increment.

    void setInc(double smallInc, double largeInc);

    //! Increment the value.

    void smallIncAction();

    //! Increment the value.

    void largeIncAction();

    //! Decrement the value.

    void smallDecAction();

    //! Decrement the value.

    void largeDecAction();

    //! Set whether clamping is enabled.

    void setClamp(bool);

    //! Set the size string.

    void setSizeString(const QString &);

Q_SIGNALS:

    //! This signal is emitted when the value is changed.

    void valueChanged(double);

    //! This signal is emitted when the default value is changed.

    void defaultValueChanged(double);

    //! This signal is emitted when the default value is valid is changed.

    void defaultValidChanged(bool);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(double);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(double);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(double min, double max);

    //! This signal is emitted when the small increment is changed.

    void smallIncChanged(double);

    //! This signal is emitted when the large increment is changed.

    void largeIncChanged(double);

    //! This signal is emitted when the increment is changed.

    void incChanged(double small, double large);

    //! This signal is emitted when the size string is changed.

    void sizeStringChanged(const QString &);

private:

    void defaultValidUpdate();

    double  _value;
    bool    _isDefaultValid;
    double  _defaultValue;
    double  _min;
    double  _max;
    bool    _clamp;
    double  _smallInc;
    double  _largeInc;
    QString _sizeString;
};

//@} // djvGuiWidget

#endif // DJV_FLOAT_OBJECT_H

