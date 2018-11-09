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

#include <djvWidgetTest/ColorWidgetTest.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/ColorWidget.h>

#include <djvCore/System.h>

#include <QHBoxLayout>

namespace djv
{
    namespace WidgetTest
    {
        ColorWidgetTest::ColorWidgetTest(const QPointer<UI::UIContext> & context) :
            AbstractWidgetTest(context)
        {}

        QString ColorWidgetTest::name()
        {
            return "ColorWidgetTest";
        }

        void ColorWidgetTest::run(const QStringList & args)
        {
            QWidget * window = new QWidget;

            UI::ColorSwatch * swatch = new UI::ColorSwatch(context());
            swatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            UI::ColorWidget * widget = new UI::ColorWidget(context());

            QHBoxLayout * layout = new QHBoxLayout(window);
            layout->addWidget(swatch);
            layout->addWidget(widget);

            connect(
                widget,
                SIGNAL(colorChanged(const djv::AV::Color &)),
                swatch,
                SLOT(setColor(const djv::AV::Color &)));
            connect(
                widget,
                SIGNAL(colorChanged(const djv::AV::Color &)),
                SLOT(colorCallback(const djv::AV::Color &)));

            window->show();
        }

        void ColorWidgetTest::colorCallback(const AV::Color & color)
        {
            QStringList tmp;
            tmp << color;
            Core::System::print(QString("%1").arg(tmp.join(", ")));
        }

    } // namespace WidgetTest
} // namespace djv
