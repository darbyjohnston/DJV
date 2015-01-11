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

//! \file djvIntObject.h

#ifndef DJV_INT_OBJECT_H
#define DJV_INT_OBJECT_H

#include <djvGuiExport.h>

#include <QObject>

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvIntObject
//!
//! This class provides the base functionality for integer widgets.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvIntObject : public QObject
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
    
    //! This property holds whether the default value is valid.
    
    Q_PROPERTY(
        bool   defaultValid
        READ   isDefaultValid
        NOTIFY defaultValidChanged)
    
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
    
    //! This property holds the small increment.
    
    Q_PROPERTY(
        int    smallInc
        READ   smallInc
        WRITE  setSmallInc
        NOTIFY smallIncChanged)
    
    //! This property holds the large increment.
    
    Q_PROPERTY(
        int    largeInc
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

    //! The minimum integer value.

    static const int intMin;

    //! The maximum integer value.

    static const int intMax;

    //! Constructor.

    explicit djvIntObject(QObject * parent = 0);
    
    //! Destructor.
    
    virtual ~djvIntObject();

    //! Get the value.

    int value() const;

    //! Get the default value.

    int defaultValue() const;

    //! Get whether the default value is valid.

    bool isDefaultValid() const;

    //! Get the minimum value.

    int min() const;

    //! Get the maximum value.

    int max() const;

    //! Get the small increment.

    int smallInc() const;

    //! Get the large increment.

    int largeInc() const;

    //! Get whether clamping is enabled.

    bool hasClamp() const;

    //! Get the size string.

    const QString & sizeString() const;

public Q_SLOTS:

    //! Set the value.

    void setValue(int);

    //! Set the default value.

    void setDefaultValue(int);

    //! Set the minimum value.

    void setMin(int);

    //! Set the maximum value.

    void setMax(int);

    //! Set the value range.

    void setRange(int min, int max);

    //! Set the value to the minimum.

    void setToMin();

    //! Set the value to the maximum.

    void setToMax();

    //! Set the small increment.

    void setSmallInc(int);

    //! Set the large increment.

    void setLargeInc(int);

    //! Set the value increment.

    void setInc(int smallInc, int largeInc);

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

    void valueChanged(int);

    //! This signal is emitted when the default value is changed.

    void defaultValueChanged(int);

    //! This signal is emitted when the default value is valid is changed.

    void defaultValidChanged(bool);

    //! This signal is emitted when the minimum value is changed.

    void minChanged(int);

    //! This signal is emitted when the maximum value is changed.

    void maxChanged(int);

    //! This signal is emitted when the value range is changed.

    void rangeChanged(int min, int max);

    //! This signal is emitted when the small increment is changed.

    void smallIncChanged(int);

    //! This signal is emitted when the large increment is changed.

    void largeIncChanged(int);

    //! This signal is emitted when the increment is changed.

    void incChanged(int small, int large);

    //! This signal is emitted when the size string is changed.

    void sizeStringChanged(const QString &);

private:

    void defaultValidUpdate();

    int     _value;
    bool    _isDefaultValid;
    int     _defaultValue;
    int     _min;
    int     _max;
    bool    _clamp;
    int     _smallInc;
    int     _largeInc;
    QString _sizeString;
};

//@} // djvGuiWidget

#endif // DJV_INT_OBJECT_H

