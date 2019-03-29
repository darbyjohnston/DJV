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

#include <djvUI/Vector2iEditWidget.h>

#include <djvUI/IntEdit.h>
#include <djvUI/IntObject.h>

#include <djvCore/SignalBlocker.h>

#include <QHBoxLayout>
#include <QPointer>

namespace djv
{
    namespace UI
    {
        struct Vector2iEditWidget::Private
        {
            QPointer<IntEdit> widget;
            QPointer<IntEdit> widget2;
        };

        Vector2iEditWidget::Vector2iEditWidget(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->widget = new IntEdit;
            _p->widget->setRange(IntObject::intMin, IntObject::intMax);

            _p->widget2 = new IntEdit;
            _p->widget2->setRange(IntObject::intMin, IntObject::intMax);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->widget);
            layout->addWidget(_p->widget2);

            connect(_p->widget,
                SIGNAL(valueChanged(int)),
                SLOT(valueCallback()));
            connect(_p->widget,
                SIGNAL(rangeChanged(int, int)),
                SLOT(rangeCallback()));
            connect(_p->widget2,
                SIGNAL(valueChanged(int)),
                SLOT(valueCallback()));
            connect(_p->widget2,
                SIGNAL(rangeChanged(int, int)),
                SLOT(rangeCallback()));
        }

        Vector2iEditWidget::~Vector2iEditWidget()
        {}
        
        glm::ivec2 Vector2iEditWidget::value() const
        {
            return glm::ivec2(_p->widget->value(), _p->widget2->value());
        }

        glm::ivec2 Vector2iEditWidget::min() const
        {
            return glm::ivec2(_p->widget->min(), _p->widget2->min());
        }

        glm::ivec2 Vector2iEditWidget::max() const
        {
            return glm::ivec2(_p->widget->max(), _p->widget2->max());
        }

        void Vector2iEditWidget::setValue(const glm::ivec2 & value)
        {
            _p->widget->setValue(value.x);
            _p->widget2->setValue(value.y);
        }

        void Vector2iEditWidget::setMin(const glm::ivec2 & min)
        {
            _p->widget->setMin(min.x);
            _p->widget2->setMin(min.y);
        }

        void Vector2iEditWidget::setMax(const glm::ivec2 & max)
        {
            _p->widget->setMax(max.x);
            _p->widget2->setMax(max.y);
        }

        void Vector2iEditWidget::setRange(
            const glm::ivec2 & min,
            const glm::ivec2 & max)
        {
            _p->widget->setRange(min.x, max.x);
            _p->widget2->setRange(min.y, max.y);
        }

        void Vector2iEditWidget::valueCallback()
        {
            Q_EMIT valueChanged(value());
        }

        void Vector2iEditWidget::rangeCallback()
        {
            Q_EMIT rangeChanged(min(), max());
        }

    } // namespace UI
} // namespace djv
