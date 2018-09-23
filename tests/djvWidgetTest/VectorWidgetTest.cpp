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

#include <djvWidgetTest/VectorWidgetTest.h>

#include <djvUI/Vector2iEditWidget.h>
#include <djvUI/Vector2fEditWidget.h>

#include <djvCore/System.h>
#include <djvCore/Vector.h>

#include <QFormLayout>

namespace djv
{
    namespace WidgetTest
    {
        VectorWidgetTest::VectorWidgetTest(const QPointer<UI::UIContext> & context) :
            AbstractWidgetTest(context)
        {}

        QString VectorWidgetTest::name()
        {
            return "VectorWidgetTest";
        }

        void VectorWidgetTest::run(const QStringList & args)
        {
            QWidget * window = new QWidget;
            UI::Vector2iEditWidget * intEdit = new UI::Vector2iEditWidget;
            UI::Vector2fEditWidget * floatEdit = new UI::Vector2fEditWidget;

            QFormLayout * layout = new QFormLayout(window);
            layout->addRow("Vector2iEditWidget", intEdit);
            layout->addRow("Vector2fEditWidget", floatEdit);

            connect(
                intEdit,
                SIGNAL(valueChanged(const glm::ivec2 &)),
                SLOT(intCallback(const glm::ivec2 &)));
            connect(
                floatEdit,
                SIGNAL(valueChanged(const glm::vec2 &)),
                SLOT(floatCallback(const glm::vec2 &)));

            window->show();
        }

        void VectorWidgetTest::intCallback(const glm::ivec2 & value)
        {
            QStringList tmp;
            tmp << value;
            Core::System::print(QString("%1").arg(tmp.join(", ")));
        }

        void VectorWidgetTest::floatCallback(const glm::vec2 & value)
        {
            QStringList tmp;
            tmp << value;
            Core::System::print(QString("%1").arg(tmp.join(", ")));
        }

    } // namespace WidgetTest
} // namespace djv
