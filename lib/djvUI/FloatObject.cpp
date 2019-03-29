//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

        struct FloatObject::Private
        {
            float   value = 0.f;
            bool    isDefaultValid = false;
            float   defaultValue = 0.f;
            float   min = floatMin;
            float   max = floatMax;
            bool    clamp = true;
            float   smallInc = 0.f;
            float   largeInc = 0.f;
            QString sizeString = "00000";
        };

        FloatObject::FloatObject(QObject * parent) :
            QObject(parent),
            _p(new Private)
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
            return _p->value;
        }

        float FloatObject::defaultValue() const
        {
            return _p->defaultValue;
        }

        bool FloatObject::isDefaultValid() const
        {
            return _p->isDefaultValid;
        }

        float FloatObject::min() const
        {
            return _p->min;
        }

        float FloatObject::max() const
        {
            return _p->max;
        }

        float FloatObject::smallInc() const
        {
            return _p->smallInc;
        }

        float FloatObject::largeInc() const
        {
            return _p->largeInc;
        }

        bool FloatObject::hasClamp() const
        {
            return _p->clamp;
        }

        const QString & FloatObject::sizeString() const
        {
            return _p->sizeString;
        }

        void FloatObject::setValue(float in)
        {
            const float tmp = _p->clamp ? Core::Math::clamp(in, _p->min, _p->max) : in;
            if (Core::Math::fuzzyCompare(tmp, _p->value))
                return;
            _p->value = tmp;
            defaultValidUpdate();
            Q_EMIT valueChanged(_p->value);
        }

        void FloatObject::setDefaultValue(float in)
        {
            if (Core::Math::fuzzyCompare(in, _p->defaultValue))
                return;
            //DJV_DEBUG("FloatObject::setDefaultValue");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->defaultValue = in;
            defaultValidUpdate();
            Q_EMIT defaultValueChanged(_p->defaultValue);
        }

        void FloatObject::setMin(float min)
        {
            setRange(min, _p->max);
        }

        void FloatObject::setMax(float max)
        {
            setRange(_p->min, max);
        }

        void FloatObject::setRange(float min, float max)
        {
            const bool minChange = !Core::Math::fuzzyCompare(min, _p->min);
            const bool maxChange = !Core::Math::fuzzyCompare(max, _p->max);
            if (!(minChange || maxChange))
                return;
            _p->min = min;
            _p->max = max;
            setValue(_p->value);
            if (minChange)
                Q_EMIT minChanged(_p->min);
            if (maxChange)
                Q_EMIT minChanged(_p->max);
            Q_EMIT rangeChanged(_p->min, _p->max);
        }

        void FloatObject::setToMin()
        {
            setValue(_p->min);
        }

        void FloatObject::setToMax()
        {
            setValue(_p->max);
        }

        void FloatObject::setSmallInc(float inc)
        {
            setInc(inc, _p->largeInc);
        }

        void FloatObject::setLargeInc(float inc)
        {
            setInc(_p->smallInc, inc);
        }

        void FloatObject::setInc(float smallInc, float largeInc)
        {
            const bool smallChange = !Core::Math::fuzzyCompare(smallInc, _p->smallInc);
            const bool largeChange = !Core::Math::fuzzyCompare(largeInc, _p->largeInc);
            if (!(smallChange || largeChange))
                return;
            _p->smallInc = smallInc;
            _p->largeInc = largeInc;
            if (smallChange)
                Q_EMIT smallIncChanged(_p->smallInc);
            if (largeChange)
                Q_EMIT largeIncChanged(_p->largeInc);
            Q_EMIT incChanged(_p->smallInc, _p->largeInc);
        }

        void FloatObject::smallIncAction()
        {
            setValue(_p->value + _p->smallInc);
        }

        void FloatObject::largeIncAction()
        {
            setValue(_p->value + _p->largeInc);
        }

        void FloatObject::smallDecAction()
        {
            setValue(_p->value - _p->smallInc);
        }

        void FloatObject::largeDecAction()
        {
            setValue(_p->value - _p->largeInc);
        }

        void FloatObject::setClamp(bool in)
        {
            if (in == _p->clamp)
                return;
            _p->clamp = in;
            setValue(_p->value);
        }

        void FloatObject::setSizeString(const QString & string)
        {
            if (string == _p->sizeString)
                return;
            _p->sizeString = string;
            Q_EMIT sizeStringChanged(_p->sizeString);
        }

        void FloatObject::defaultValidUpdate()
        {
            const bool defaultValid = Core::Math::fuzzyCompare(_p->value, _p->defaultValue);
            if (defaultValid != _p->isDefaultValid)
            {
                _p->isDefaultValid = defaultValid;
                Q_EMIT defaultValidChanged(_p->isDefaultValid);
            }
        }

    } // namespace UI
} // namespace djv
