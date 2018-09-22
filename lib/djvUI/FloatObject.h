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

#pragma once

#include <djvUI/Core.h>

#include <QObject>

namespace djv
{
    namespace UI
    {
        //! \class FloatObject
        //!
        //! This class provides the base functionality for floating point widgets.
        class FloatObject : public QObject
        {
            Q_OBJECT

                //! This property holds the value.
                Q_PROPERTY(
                    float  value
                    READ   value
                    WRITE  setValue
                    NOTIFY valueChanged)

                //! This property holds the default value.
                Q_PROPERTY(
                    float  defaultValue
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
                    float  min
                    READ   min
                    WRITE  setMin
                    NOTIFY minChanged)

                //! This property holds the maximum value.
                Q_PROPERTY(
                    float  max
                    READ   max
                    WRITE  setMax
                    NOTIFY maxChanged)

                //! This property holds the small increment.
                Q_PROPERTY(
                    float  smallInc
                    READ   smallInc
                    WRITE  setSmallInc
                    NOTIFY smallIncChanged)

                //! This property holds the large increment.
                Q_PROPERTY(
                    float  largeInc
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
            static const float floatMin;

            //! The maximum floating point value.
            static const float floatMax;

            explicit FloatObject(QObject * parent = nullptr);

            //! Get the value.
            float value() const;

            //! Get the default value.
            float defaultValue() const;

            //! Get whether the default value is valid.
            bool isDefaultValid() const;

            //! Get the minimum value.
            float min() const;

            //! Get the maximum value.
            float max() const;

            //! Get the small increment.
            float smallInc() const;

            //! Get the large increment.
            float largeInc() const;

            //! Get whether clamping is enabled.
            bool hasClamp() const;

            //! Get the size string.
            const QString & sizeString() const;

        public Q_SLOTS:
            //! Set the value.
            void setValue(float);

            //! Set the default value.
            void setDefaultValue(float);

            //! Set the minimum value.
            void setMin(float);

            //! Set the maximum value.
            void setMax(float);

            //! Set the value range.
            void setRange(float min, float max);

            //! Set the value to the minimum.
            void setToMin();

            //! Set the value to the maximum.
            void setToMax();

            //! Set the small increment.
            void setSmallInc(float);

            //! Set the large increment.
            void setLargeInc(float);

            //! Set the value increment.
            void setInc(float smallInc, float largeInc);

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
            void valueChanged(float);

            //! This signal is emitted when the default value is changed.
            void defaultValueChanged(float);

            //! This signal is emitted when the default value is valid is changed.
            void defaultValidChanged(bool);

            //! This signal is emitted when the minimum value is changed.
            void minChanged(float);

            //! This signal is emitted when the maximum value is changed.
            void maxChanged(float);

            //! This signal is emitted when the value range is changed.
            void rangeChanged(float min, float max);

            //! This signal is emitted when the small increment is changed.
            void smallIncChanged(float);

            //! This signal is emitted when the large increment is changed.
            void largeIncChanged(float);

            //! This signal is emitted when the increment is changed.
            void incChanged(float, float);

            //! This signal is emitted when the size string is changed.
            void sizeStringChanged(const QString &);

        private:
            void defaultValidUpdate();

            float   _value = 0.f;
            bool    _isDefaultValid = false;
            float   _defaultValue = 0.f;
            float   _min = 0.f;
            float   _max = 0.f;
            bool    _clamp = true;
            float   _smallInc = 0.f;
            float   _largeInc = 0.f;
            QString _sizeString = "00000";
        };

    } // namespace UI
} // namespace djv
