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

#include <djvUI/IntEdit.h>

#include <djvUI/IntObject.h>

#include <djvCore/SignalBlocker.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QSpinBox>

namespace djv
{
    namespace UI
    {
        struct IntEdit::Private
        {
            IntObject * object = nullptr;
            QSpinBox * spinBox = nullptr;
        };

        IntEdit::IntEdit(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->object = new IntObject(this);

            _p->spinBox = new QSpinBox;
            _p->spinBox->setKeyboardTracking(false);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->spinBox);

            _p->object->setRange(1, 100);

            widgetUpdate();

            connect(
                _p->object,
                SIGNAL(valueChanged(int)),
                SLOT(valueCallback()));
            connect(
                _p->object,
                SIGNAL(minChanged(int)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(maxChanged(int)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(rangeChanged(int, int)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(incChanged(int, int)),
                SLOT(widgetUpdate()));
            connect(
                _p->spinBox,
                SIGNAL(valueChanged(int)),
                SLOT(spinBoxCallback(int)));
        }

        IntEdit::~IntEdit()
        {}
        
        int IntEdit::value() const
        {
            return _p->object->value();
        }

        int IntEdit::min() const
        {
            return _p->object->min();
        }

        int IntEdit::max() const
        {
            return _p->object->max();
        }

        IntObject * IntEdit::object() const
        {
            return _p->object;
        }

        void IntEdit::setValue(int value)
        {
            _p->object->setValue(value);
        }

        void IntEdit::setMin(int min)
        {
            _p->object->setMin(min);
        }

        void IntEdit::setMax(int max)
        {
            _p->object->setMax(max);
        }

        void IntEdit::setRange(int min, int max)
        {
            _p->object->setRange(min, max);
        }

        void IntEdit::keyPressEvent(QKeyEvent * event)
        {
            switch (event->key())
            {
            case Qt::Key_Home:     _p->object->setToMin();       break;
            case Qt::Key_End:      _p->object->setToMax();       break;
            case Qt::Key_Left:
            case Qt::Key_Down:     _p->object->smallDecAction(); break;
            case Qt::Key_Right:
            case Qt::Key_Up:       _p->object->smallIncAction(); break;
            case Qt::Key_PageDown: _p->object->largeDecAction(); break;
            case Qt::Key_PageUp:   _p->object->largeIncAction(); break;
            default: break;
            }
        }

        void IntEdit::valueCallback()
        {
            widgetUpdate();
            Q_EMIT valueChanged(_p->object->value());
        }

        void IntEdit::rangeCallback()
        {
            widgetUpdate();
            Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
        }

        void IntEdit::spinBoxCallback(int value)
        {
            setValue(value);
        }

        void IntEdit::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(_p->spinBox);
            _p->spinBox->setRange(_p->object->min(), _p->object->max());
            _p->spinBox->setSingleStep(_p->object->smallInc());
            _p->spinBox->setValue(_p->object->value());
        }

    } // namespace UI
} // namespace djv
