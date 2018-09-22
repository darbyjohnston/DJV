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

#include <djvCore/Util.h>

#include <QWidget>

#include <memory>

namespace djv
{
    namespace UI
    {
        class FloatObject;

        //! \class FloatEdit
        //!
        //! This class provides a floating-point edit widget.
        class FloatEdit : public QWidget
        {
            Q_OBJECT

            //! This property holds the value.
            Q_PROPERTY(
                float  value
                READ   value
                WRITE  setValue
                NOTIFY valueChanged)

        public:
            explicit FloatEdit(QWidget * parent = nullptr);
            ~FloatEdit() override;
            
            //! Get the value.
            float value() const;

            //! Get the minimum value.
            float min() const;

            //! Get the maximum value.
            float max() const;

            //! Get the floating-point object.
            FloatObject * object() const;

        public Q_SLOTS:
            //! Set the value.
            void setValue(float);

            //! Set the minimum value.
            void setMin(float);

            //! Set the maximum value.
            void setMax(float);

            //! Set the value range.
            void setRange(float min, float max);

        Q_SIGNALS:
            //! This signal is emitted when the value is changed.
            void valueChanged(float);

            //! This signal is emitted when the minimum value is changed.
            void minChanged(float);

            //! This signal is emitted when the maximum value is changed.
            void maxChanged(float);

            //! This signal is emitted when the value range is changed.
            void rangeChanged(float, float);

        protected:
            void keyPressEvent(QKeyEvent *) override;

        private Q_SLOTS:
            void valueCallback();
            void rangeCallback();
            void spinBoxCallback(double);

            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(FloatEdit);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
