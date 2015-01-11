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

//! \file djvIntObject.cpp

#include <djvIntObject.h>

#include <djvDebug.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvIntObject
//------------------------------------------------------------------------------

const int djvIntObject::intMin = -1000000;
const int djvIntObject::intMax =  1000000;

djvIntObject::djvIntObject(QObject * parent) :
    QObject(parent),
    _value         (0),
    _isDefaultValid(false),
    _defaultValue  (0),
    _min           (intMin),
    _max           (intMax),
    _clamp         (true),
    _smallInc      (0),
    _largeInc      (0),
    _sizeString    ("00000")
{
    setValue(0);
    
    setDefaultValue(0);
    
    setInc(1, 10);

    defaultValidUpdate();
}

djvIntObject::~djvIntObject()
{}

int djvIntObject::value() const
{
    return _value;
}

int djvIntObject::defaultValue() const
{
    return _defaultValue;
}

bool djvIntObject::isDefaultValid() const
{
    return _isDefaultValid;
}

int djvIntObject::min() const
{
    return _min;
}

int djvIntObject::max() const
{
    return _max;
}

int djvIntObject::smallInc() const
{
    return _smallInc;
}

int djvIntObject::largeInc() const
{
    return _largeInc;
}

bool djvIntObject::hasClamp() const
{
    return _clamp;
}

const QString & djvIntObject::sizeString() const
{
    return _sizeString;
}

void djvIntObject::setValue(int in)
{
    const int tmp = _clamp ? djvMath::clamp(in, _min, _max) : in;

    if (tmp == _value)
        return;

    _value = tmp;

    defaultValidUpdate();

    Q_EMIT valueChanged(_value);
}

void djvIntObject::setDefaultValue(int in)
{
    if (in == _defaultValue)
        return;

    _defaultValue = in;

    defaultValidUpdate();

    Q_EMIT defaultValueChanged(_defaultValue);
}

void djvIntObject::setMin(int min)
{
    setRange(min, _max);
}

void djvIntObject::setMax(int max)
{
    setRange(_min, max);
}

void djvIntObject::setRange(int min, int max)
{
    const bool minChange = min != _min;
    const bool maxChange = max != _max;
    
    if (! (minChange || maxChange))
        return;

    //DJV_DEBUG("djvIntObject::setRange");
    //DJV_DEBUG_PRINT("min = " << min);
    //DJV_DEBUG_PRINT("max = " << max);

    _min = min;
    _max = max;

    setValue(_value);

    if (minChange)
        Q_EMIT minChanged(_min);
    if (maxChange)
        Q_EMIT minChanged(_max);
    Q_EMIT rangeChanged(_min, _max);
}

void djvIntObject::setToMin()
{
    setValue(_min);
}

void djvIntObject::setToMax()
{
    setValue(_max);
}

void djvIntObject::setSmallInc(int inc)
{
    setInc(inc, _largeInc);
}

void djvIntObject::setLargeInc(int inc)
{
    setInc(_smallInc, inc);
}

void djvIntObject::setInc(int smallInc, int largeInc)
{
    const bool smallChange = smallInc != _smallInc;
    const bool largeChange = largeInc != _largeInc;
    
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

void djvIntObject::smallIncAction()
{
    setValue(_value + _smallInc);
}

void djvIntObject::largeIncAction()
{
    setValue(_value + _largeInc);
}

void djvIntObject::smallDecAction()
{
    setValue(_value - _smallInc);
}

void djvIntObject::largeDecAction()
{
    setValue(_value - _largeInc);
}

void djvIntObject::setClamp(bool in)
{
    if (in == _clamp)
        return;

    _clamp = in;

    setValue(_value);
}

void djvIntObject::setSizeString(const QString & string)
{
    if (string == _sizeString)
        return;

    _sizeString = string;

    Q_EMIT sizeStringChanged(_sizeString);
}

void djvIntObject::defaultValidUpdate()
{
    const bool defaultValid = _value == _defaultValue;

    if (defaultValid != _isDefaultValid)
    {
        _isDefaultValid = defaultValid;

        Q_EMIT defaultValidChanged(_isDefaultValid);
    }
}
