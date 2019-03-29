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

#include <djvWidgetTest/WidgetTestApplication.h>

#include <djvWidgetTest/AbstractWidgetTest.h>

#include <djvUI/ProxyStyle.h>

#include <QPointer>

namespace djv
{
    namespace WidgetTest
    {
        WidgetTestApplication::WidgetTestApplication(int & argc, char ** argv) :
            QApplication(argc, argv)
        {
            _context.reset(new UI::UIContext(argc, argv));
            _context->setValid(true);

            setStyle(new UI::ProxyStyle(_context.data()));

            _testManager.reset(new WidgetTestManager(_context.data()));

            _model.reset(new WidgetTestModel(_testManager.data()));

            _window.reset(new WidgetTestWindow(_model.data(), _context.data()));
            _window->show();

            if (argc > 1)
            {
                QStringList args;
                for (int i = 2; i < argc; ++i)
                {
                    args += QString(argv[i]);
                }
                for (int i = 0; i < _testManager->tests().count(); ++i)
                {
                    if (argv[1] == _testManager->tests()[i]->name())
                    {
                        _testManager->tests()[i]->run(args);
                        break;
                    }
                }
            }
        }

    } // namespace WidgetTest
} // namespace djv

int main(int argc, char ** argv)
{
    djv::Core::CoreContext::initLibPaths(argc, argv);
    return djv::WidgetTest::WidgetTestApplication(argc, argv).exec();
}

