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

//! \file djvFloatObject.cpp

#include <djvFloatObject.h>

#include <djvDebug.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvFloatObject
//------------------------------------------------------------------------------

const double djvFloatObject::floatMin = -9999.0;
const double djvFloatObject::floatMax =  9999.0;

djvFloatObject::djvFloatObject(QObject * parent) :
    QObject(parent),
    _value         (0.0),
    _isDefaultValid(false),
    _defaultValue  (0.0),
    _min           (floatMin),
    _max           (floatMax),
    _clamp         (true),
    _smallInc      (0.0),
    _largeInc      (0.0),
    _sizeString    ("00000")
{
    setValue(0.0);

    setDefaultValue(0.0);

    setInc(0.1, 1.0);

    defaultValidUpdate();
}

djvFloatObject::~djvFloatObject()
{}

double djvFloatObject::value() const
{
    return _value;
}

double djvFloatObject::defaultValue() const
{
    return _defaultValue;
}

bool djvFloatObject::isDefaultValid() const
{
    return _isDefaultValid;
}

double djvFloatObject::min() const
{
    return _min;
}

double djvFloatObject::max() const
{
    return _max;
}

double djvFloatObject::smallInc() const
{
    return _smallInc;
}

double djvFloatObject::largeInc() const
{
    return _largeInc;
}

bool djvFloatObject::hasClamp() const
{
    return _clamp;
}

const QString & djvFloatObject::sizeString() const
{
    return _sizeString;
}

void djvFloatObject::setValue(double in)
{
    const double tmp = _clamp ? djvMath::clamp(in, _min, _max) : in;

    if (djvMath::fuzzyCompare(tmp, _value))
        return;

    _value = tmp;

    defaultValidUpdate();

    Q_EMIT valueChanged(_value);
}

void djvFloatObject::setDefaultValue(double in)
{
    if (djvMath::fuzzyCompare(in, _defaultValue))
        return;

    //DJV_DEBUG("djvFloatObject::setDefaultValue");
    //DJV_DEBUG_PRINT("in = " << in);

    _defaultValue = in;

    defaultValidUpdate();

    Q_EMIT defaultValueChanged(_defaultValue);
}

void djvFloatObject::setMin(double min)
{
    setRange(min, _max);
}

void djvFloatObject::setMax(double max)
{
    setRange(_min, max);
}

void djvFloatObject::setRange(double min, double max)
{
    const bool minChange = ! djvMath::fuzzyCompare(min, _min);
    const bool maxChange = ! djvMath::fuzzyCompare(max, _max);
    
    if (! (minChange || maxChange))
        return;

    _min = min;
    _max = max;

    setValue(_value);

    if (minChange)
        Q_EMIT minChanged(_min);
    if (maxChange)
        Q_EMIT minChanged(_max);
    Q_EMIT rangeChanged(_min, _max);
}

void djvFloatObject::setToMin()
{
    setValue(_min);
}

void djvFloatObject::setToMax()
{
    setValue(_max);
}

void djvFloatObject::setSmallInc(double inc)
{
    setInc(inc, _largeInc);
}

void djvFloatObject::setLargeInc(double inc)
{
    setInc(_smallInc, inc);
}

void djvFloatObject::setInc(double smallInc, double largeInc)
{
    const bool smallChange = ! djvMath::fuzzyCompare(smallInc, _smallInc);
    const bool largeChange = ! djvMath::fuzzyCompare(largeInc, _largeInc);
    
    if (! (smallChange || largeChange))
        return;

    _smallInc = smallInc;
    _largeInc = largeInc;

    if (smallChange)
        Q_EMIT smallIncChanged(_smallInc);
    if (largeChange)
        Q_EMIT largeIncChanged(_largeInc);
    Q_EMIT incChanged(_smallInc, _largeInc);
}

void djvFloatObject::smallIncAction()
{
    setValue(_value + _smallInc);
}

void djvFloatObject::largeIncAction()
{
    setValue(_value + _largeInc);
}

void djvFloatObject::smallDecAction()
{
    setValue(_value - _smallInc);
}

void djvFloatObject::largeDecAction()
{
    setValue(_value - _largeInc);
}

void djvFloatObject::setClamp(bool in)
{
    if (in == _clamp)
        return;

    _clamp = in;

    setValue(_value);
}

void djvFloatObject::setSizeString(const QString & string)
{
    if (string == _sizeString)
        return;

    _sizeString = string;

    Q_EMIT sizeStringChanged(_sizeString);
}

void djvFloatObject::defaultValidUpdate()
{
    const bool defaultValid = djvMath::fuzzyCompare(_value, _defaultValue);

    if (defaultValid != _isDefaultValid)
    {
        _isDefaultValid = defaultValid;

        Q_EMIT defaultValidChanged(_isDefaultValid);
    }
}
