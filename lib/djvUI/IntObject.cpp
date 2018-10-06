//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/IntObject.h>

#include <djvCore/Debug.h>
#include <djvCore/Math.h>

namespace djv
{
    namespace UI
    {
        const int IntObject::intMin = -1000000;
        const int IntObject::intMax = 1000000;

        struct IntObject::Private
        {
            int     value = 0;
            bool    isDefaultValid = false;
            int     defaultValue = 0;
            int     min = intMin;
            int     max = intMax;
            bool    clamp = true;
            int     smallInc = 0;
            int     largeInc = 0;
            QString sizeString = "00000";
        };

        IntObject::IntObject(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            setValue(0);
            setDefaultValue(0);
            setInc(1, 10);
            defaultValidUpdate();
        }

        IntObject::~IntObject()
        {}
        
        int IntObject::value() const
        {
            return _p->value;
        }

        int IntObject::defaultValue() const
        {
            return _p->defaultValue;
        }

        bool IntObject::isDefaultValid() const
        {
            return _p->isDefaultValid;
        }

        int IntObject::min() const
        {
            return _p->min;
        }

        int IntObject::max() const
        {
            return _p->max;
        }

        int IntObject::smallInc() const
        {
            return _p->smallInc;
        }

        int IntObject::largeInc() const
        {
            return _p->largeInc;
        }

        bool IntObject::hasClamp() const
        {
            return _p->clamp;
        }

        const QString & IntObject::sizeString() const
        {
            return _p->sizeString;
        }

        void IntObject::setValue(int in)
        {
            const int tmp = _p->clamp ? Core::Math::clamp(in, _p->min, _p->max) : in;
            if (tmp == _p->value)
                return;
            _p->value = tmp;
            defaultValidUpdate();
            Q_EMIT valueChanged(_p->value);
        }

        void IntObject::setDefaultValue(int in)
        {
            if (in == _p->defaultValue)
                return;
            _p->defaultValue = in;
            defaultValidUpdate();
            Q_EMIT defaultValueChanged(_p->defaultValue);
        }

        void IntObject::setMin(int min)
        {
            setRange(min, _p->max);
        }

        void IntObject::setMax(int max)
        {
            setRange(_p->min, max);
        }

        void IntObject::setRange(int min, int max)
        {
            const bool minChange = min != _p->min;
            const bool maxChange = max != _p->max;
            if (!(minChange || maxChange))
                return;
            //DJV_DEBUG("IntObject::setRange");
            //DJV_DEBUG_PRINT("min = " << min);
            //DJV_DEBUG_PRINT("max = " << max);
            _p->min = min;
            _p->max = max;
            setValue(_p->value);
            if (minChange)
                Q_EMIT minChanged(_p->min);
            if (maxChange)
                Q_EMIT minChanged(_p->max);
            Q_EMIT rangeChanged(_p->min, _p->max);
        }

        void IntObject::setToMin()
        {
            setValue(_p->min);
        }

        void IntObject::setToMax()
        {
            setValue(_p->max);
        }

        void IntObject::setSmallInc(int inc)
        {
            setInc(inc, _p->largeInc);
        }

        void IntObject::setLargeInc(int inc)
        {
            setInc(_p->smallInc, inc);
        }

        void IntObject::setInc(int smallInc, int largeInc)
        {
            const bool smallChange = smallInc != _p->smallInc;
            const bool largeChange = largeInc != _p->largeInc;
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

        void IntObject::smallIncAction()
        {
            setValue(_p->value + _p->smallInc);
        }

        void IntObject::largeIncAction()
        {
            setValue(_p->value + _p->largeInc);
        }

        void IntObject::smallDecAction()
        {
            setValue(_p->value - _p->smallInc);
        }

        void IntObject::largeDecAction()
        {
            setValue(_p->value - _p->largeInc);
        }

        void IntObject::setClamp(bool in)
        {
            if (in == _p->clamp)
                return;
            _p->clamp = in;
            setValue(_p->value);
        }

        void IntObject::setSizeString(const QString & string)
        {
            if (string == _p->sizeString)
                return;
            _p->sizeString = string;
            Q_EMIT sizeStringChanged(_p->sizeString);
        }

        void IntObject::defaultValidUpdate()
        {
            const bool defaultValid = _p->value == _p->defaultValue;
            if (defaultValid != _p->isDefaultValid)
            {
                _p->isDefaultValid = defaultValid;
                Q_EMIT defaultValidChanged(_p->isDefaultValid);
            }
        }

    } // namespace UI
} // namespace djv
