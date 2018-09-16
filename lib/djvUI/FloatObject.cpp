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

#include <djvUI/FloatObject.h>

#include <djvCore/Debug.h>
#include <djvCore/Math.h>

namespace djv
{
    namespace UI
    {
        const float FloatObject::floatMin = -9999.f;
        const float FloatObject::floatMax = 9999.f;

        FloatObject::FloatObject(QObject * parent) :
            QObject(parent),
            _min(floatMin),
            _max(floatMax)
        {
            setValue(0.f);
            setDefaultValue(0.f);
            setInc(0.1f, 1.f);
            defaultValidUpdate();
        }

        FloatObject::~FloatObject()
        {}

        float FloatObject::value() const
        {
            return _value;
        }

        float FloatObject::defaultValue() const
        {
            return _defaultValue;
        }

        bool FloatObject::isDefaultValid() const
        {
            return _isDefaultValid;
        }

        float FloatObject::min() const
        {
            return _min;
        }

        float FloatObject::max() const
        {
            return _max;
        }

        float FloatObject::smallInc() const
        {
            return _smallInc;
        }

        float FloatObject::largeInc() const
        {
            return _largeInc;
        }

        bool FloatObject::hasClamp() const
        {
            return _clamp;
        }

        const QString & FloatObject::sizeString() const
        {
            return _sizeString;
        }

        void FloatObject::setValue(float in)
        {
            const float tmp = _clamp ? Core::Math::clamp(in, _min, _max) : in;
            if (Core::Math::fuzzyCompare(tmp, _value))
                return;
            _value = tmp;
            defaultValidUpdate();
            Q_EMIT valueChanged(_value);
        }

        void FloatObject::setDefaultValue(float in)
        {
            if (Core::Math::fuzzyCompare(in, _defaultValue))
                return;
            //DJV_DEBUG("FloatObject::setDefaultValue");
            //DJV_DEBUG_PRINT("in = " << in);
            _defaultValue = in;
            defaultValidUpdate();
            Q_EMIT defaultValueChanged(_defaultValue);
        }

        void FloatObject::setMin(float min)
        {
            setRange(min, _max);
        }

        void FloatObject::setMax(float max)
        {
            setRange(_min, max);
        }

        void FloatObject::setRange(float min, float max)
        {
            const bool minChange = !Core::Math::fuzzyCompare(min, _min);
            const bool maxChange = !Core::Math::fuzzyCompare(max, _max);
            if (!(minChange || maxChange))
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

        void FloatObject::setToMin()
        {
            setValue(_min);
        }

        void FloatObject::setToMax()
        {
            setValue(_max);
        }

        void FloatObject::setSmallInc(float inc)
        {
            setInc(inc, _largeInc);
        }

        void FloatObject::setLargeInc(float inc)
        {
            setInc(_smallInc, inc);
        }

        void FloatObject::setInc(float smallInc, float largeInc)
        {
            const bool smallChange = !Core::Math::fuzzyCompare(smallInc, _smallInc);
            const bool largeChange = !Core::Math::fuzzyCompare(largeInc, _largeInc);
            if (!(smallChange || largeChange))
                return;
            _smallInc = smallInc;
            _largeInc = largeInc;
            if (smallChange)
                Q_EMIT smallIncChanged(_smallInc);
            if (largeChange)
                Q_EMIT largeIncChanged(_largeInc);
            Q_EMIT incChanged(_smallInc, _largeInc);
        }

        void FloatObject::smallIncAction()
        {
            setValue(_value + _smallInc);
        }

        void FloatObject::largeIncAction()
        {
            setValue(_value + _largeInc);
        }

        void FloatObject::smallDecAction()
        {
            setValue(_value - _smallInc);
        }

        void FloatObject::largeDecAction()
        {
            setValue(_value - _largeInc);
        }

        void FloatObject::setClamp(bool in)
        {
            if (in == _clamp)
                return;
            _clamp = in;
            setValue(_value);
        }

        void FloatObject::setSizeString(const QString & string)
        {
            if (string == _sizeString)
                return;
            _sizeString = string;
            Q_EMIT sizeStringChanged(_sizeString);
        }

        void FloatObject::defaultValidUpdate()
        {
            const bool defaultValid = Core::Math::fuzzyCompare(_value, _defaultValue);
            if (defaultValid != _isDefaultValid)
            {
                _isDefaultValid = defaultValid;
                Q_EMIT defaultValidChanged(_isDefaultValid);
            }
        }

    } // namespace UI
} // namespace djv
