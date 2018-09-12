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

#include <djvUI/Vector2fEditWidget.h>

#include <djvUI/FloatEdit.h>
#include <djvUI/FloatObject.h>

#include <djvCore/SignalBlocker.h>

#include <QHBoxLayout>

namespace djv
{
    namespace UI
    {
        struct Vector2fEditWidget::Private
        {
            FloatEdit * widget = nullptr;
            FloatEdit * widget2 = nullptr;
        };

        Vector2fEditWidget::Vector2fEditWidget(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->widget = new FloatEdit;
            _p->widget->setRange(FloatObject::floatMin, FloatObject::floatMax);

            _p->widget2 = new FloatEdit;
            _p->widget2->setRange(FloatObject::floatMin, FloatObject::floatMax);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->widget);
            layout->addWidget(_p->widget2);

            connect(
                _p->widget,
                SIGNAL(valueChanged(float)),
                SLOT(valueCallback()));
            connect(
                _p->widget,
                SIGNAL(rangeChanged(float, float)),
                SLOT(rangeCallback()));
            connect(
                _p->widget2,
                SIGNAL(valueChanged(float)),
                SLOT(valueCallback()));
            connect(
                _p->widget2,
                SIGNAL(rangeChanged(float, float)),
                SLOT(rangeCallback()));
        }

        Vector2fEditWidget::~Vector2fEditWidget()
        {}

        glm::vec2 Vector2fEditWidget::value() const
        {
            return glm::vec2(_p->widget->value(), _p->widget2->value());
        }

        glm::vec2 Vector2fEditWidget::min() const
        {
            return glm::vec2(_p->widget->min(), _p->widget2->min());
        }

        glm::vec2 Vector2fEditWidget::max() const
        {
            return glm::vec2(_p->widget->max(), _p->widget2->max());
        }

        void Vector2fEditWidget::setValue(const glm::vec2 & value)
        {
            _p->widget->setValue(value.x);
            _p->widget2->setValue(value.y);
        }

        void Vector2fEditWidget::setMin(const glm::vec2 & min)
        {
            _p->widget->setMin(min.x);
            _p->widget2->setMin(min.y);
        }

        void Vector2fEditWidget::setMax(const glm::vec2 & max)
        {
            _p->widget->setMax(max.x);
            _p->widget2->setMax(max.y);
        }

        void Vector2fEditWidget::setRange(
            const glm::vec2 & min,
            const glm::vec2 & max)
        {
            _p->widget->setRange(min.x, max.x);
            _p->widget2->setRange(min.y, max.y);
        }

        void Vector2fEditWidget::valueCallback()
        {
            Q_EMIT valueChanged(value());
        }

        void Vector2fEditWidget::rangeCallback()
        {
            Q_EMIT rangeChanged(min(), max());
        }

    } // namespace UI
} // namespace djv
