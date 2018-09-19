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

#include <djvWidgetTest/NumWidgetTest.h>

#include <djvUI/FloatDisplay.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatEditSlider.h>
#include <djvUI/IntDisplay.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntEditSlider.h>

#include <djvCore/SignalBlocker.h>
#include <djvCore/System.h>

#include <QFormLayout>

namespace djv
{
    namespace WidgetTest
    {
        NumWidgetTest::NumWidgetTest(UI::UIContext * context) :
            AbstractWidgetTest(context)
        {}

        QString NumWidgetTest::name()
        {
            return "NumWidgetTest";
        }

        void NumWidgetTest::run(const QStringList & args)
        {
            (new NumWidgetTestWidget(context()))->show();
        }

        NumWidgetTestWidget::NumWidgetTestWidget(UI::UIContext * context) :
            _intValue(0),
            _floatValue(0.f),
            _intEdit(0),
            _intDisplay(0),
            _intSlider(0),
            _floatEdit(0),
            _floatDisplay(0),
            _floatSlider(0)
        {
            _intEdit = new UI::IntEdit;

            _intDisplay = new UI::IntDisplay;

            _intSlider = new UI::IntEditSlider(context);

            _floatEdit = new UI::FloatEdit;

            _floatDisplay = new UI::FloatDisplay;

            _floatSlider = new UI::FloatEditSlider(context);

            QFormLayout * layout = new QFormLayout(this);
            layout->addRow("IntEdit", _intEdit);
            layout->addRow("IntDisplay", _intDisplay);
            layout->addRow("IntEditSlider", _intSlider);
            layout->addRow("FloatEdit", _floatEdit);
            layout->addRow("FloatDisplay", _floatDisplay);
            layout->addRow("FloatEditSlider", _floatSlider);

            widgetUpdate();

            connect(
                _intEdit,
                SIGNAL(valueChanged(int)),
                SLOT(intCallback(int)));
            connect(
                _intSlider,
                SIGNAL(valueChanged(int)),
                SLOT(intCallback(int)));
            connect(
                _floatEdit,
                SIGNAL(valueChanged(float)),
                SLOT(floatCallback(float)));
            connect(
                _floatSlider,
                SIGNAL(valueChanged(float)),
                SLOT(floatCallback(float)));
        }

        void NumWidgetTestWidget::intCallback(int value)
        {
            _intValue = value;
            Core::System::print(QString("%1").arg(_intValue));
            widgetUpdate();
        }

        void NumWidgetTestWidget::floatCallback(float value)
        {
            _floatValue = value;
            Core::System::print(QString("%1").arg(_floatValue));
            widgetUpdate();
        }

        void NumWidgetTestWidget::widgetUpdate()
        {
            Core::SignalBlocker SignalBlocker(QObjectList() <<
                _intEdit <<
                _intDisplay <<
                _intSlider <<
                _floatEdit <<
                _floatDisplay <<
                _floatSlider);
            _intEdit->setValue(_intValue);
            _intDisplay->setValue(_intValue);
            _intSlider->setValue(_intValue);
            _floatEdit->setValue(_floatValue);
            _floatDisplay->setValue(_floatValue);
            _floatSlider->setValue(_floatValue);
        }

    } // namespace WidgetTest
} // namespace djv
