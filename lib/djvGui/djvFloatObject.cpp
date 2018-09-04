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

#include <djvFloatObject.h>

#include <djvDebug.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvFloatObject
//------------------------------------------------------------------------------

const float djvFloatObject::floatMin = -9999.f;
const float djvFloatObject::floatMax =  9999.f;

djvFloatObject::djvFloatObject(QObject * parent) :
    QObject(parent),
    _value         (0.f),
    _isDefaultValid(false),
    _defaultValue  (0.f),
    _min           (floatMin),
    _max           (floatMax),
    _clamp         (true),
    _smallInc      (0.f),
    _largeInc      (0.f),
    _sizeString    ("00000")
{
    setValue(0.f);
    setDefaultValue(0.f);
    setInc(0.1f, 1.f);
    defaultValidUpdate();
}

djvFloatObject::~djvFloatObject()
{}

float djvFloatObject::value() const
{
    return _value;
}

float djvFloatObject::defaultValue() const
{
    return _defaultValue;
}

bool djvFloatObject::isDefaultValid() const
{
    return _isDefaultValid;
}

float djvFloatObject::min() const
{
    return _min;
}

float djvFloatObject::max() const
{
    return _max;
}

float djvFloatObject::smallInc() const
{
    return _smallInc;
}

float djvFloatObject::largeInc() const
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

void djvFloatObject::setValue(float in)
{
    const float tmp = _clamp ? djvMath::clamp(in, _min, _max) : in;
    if (djvMath::fuzzyCompare(tmp, _value))
        return;
    _value = tmp;
    defaultValidUpdate();
    Q_EMIT valueChanged(_value);
}

void djvFloatObject::setDefaultValue(float in)
{
    if (djvMath::fuzzyCompare(in, _defaultValue))
        return;
    //DJV_DEBUG("djvFloatObject::setDefaultValue");
    //DJV_DEBUG_PRINT("in = " << in);
    _defaultValue = in;
    defaultValidUpdate();
    Q_EMIT defaultValueChanged(_defaultValue);
}

void djvFloatObject::setMin(float min)
{
    setRange(min, _max);
}

void djvFloatObject::setMax(float max)
{
    setRange(_min, max);
}

void djvFloatObject::setRange(float min, float max)
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

void djvFloatObject::setSmallInc(float inc)
{
    setInc(inc, _largeInc);
}

void djvFloatObject::setLargeInc(float inc)
{
    setInc(_smallInc, inc);
}

void djvFloatObject::setInc(float smallInc, float largeInc)
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
