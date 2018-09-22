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

#include <djvWidgetTest/WidgetTestManager.h>

#include <djvWidgetTest/AboutDialogTest.h>
#include <djvWidgetTest/ChoiceDialogTest.h>
#include <djvWidgetTest/ColorWidgetTest.h>
#include <djvWidgetTest/ColorDialogTest.h>
#include <djvWidgetTest/FileBrowserTest.h>
#include <djvWidgetTest/FileEditTest.h>
#include <djvWidgetTest/IconLibraryTest.h>
#include <djvWidgetTest/InfoDialogTest.h>
#include <djvWidgetTest/InputDialogTest.h>
#include <djvWidgetTest/IntSliderTest.h>
#include <djvWidgetTest/MessagesDialogTest.h>
#include <djvWidgetTest/MultiChoiceDialogTest.h>
#include <djvWidgetTest/NoticeDialogTest.h>
#include <djvWidgetTest/NumWidgetTest.h>
#include <djvWidgetTest/PixelWidgetTest.h>
#include <djvWidgetTest/PrefsDialogTest.h>
#include <djvWidgetTest/ProgressDialogTest.h>
#include <djvWidgetTest/QuestionDialogTest.h>
#include <djvWidgetTest/SearchBoxTest.h>
#include <djvWidgetTest/ShortcutsWidgetTest.h>
#include <djvWidgetTest/ToolButtonTest.h>
#include <djvWidgetTest/VectorWidgetTest.h>
#include <djvWidgetTest/ViewWidgetTest.h>

#include <djvUI/UIContext.h>

namespace djv
{
    namespace WidgetTest
    {
        WidgetTestManager::WidgetTestManager(UI::UIContext * context)
        {
            _tests += new AboutDialogTest(context);
            _tests += new ChoiceDialogTest(context);
            _tests += new ColorWidgetTest(context);
            _tests += new ColorDialogTest(context);
            _tests += new FileBrowserTest(context);
            _tests += new FileEditTest(context);
            _tests += new IconLibraryTest(context);
            _tests += new InfoDialogTest(context);
            _tests += new InputDialogTest(context);
            _tests += new IntSliderTest(context);
            _tests += new MessagesDialogTest(context);
            _tests += new MultiChoiceDialogTest(context);
            _tests += new NoticeDialogTest(context);
            _tests += new NumWidgetTest(context);
            _tests += new PixelWidgetTest(context);
            _tests += new PrefsDialogTest(context);
            _tests += new ProgressDialogTest(context);
            _tests += new QuestionDialogTest(context);
            _tests += new SearchBoxTest(context);
            _tests += new ShortcutsWidgetTest(context);
            _tests += new ToolButtonTest(context);
            _tests += new VectorWidgetTest(context);
            _tests += new ViewWidgetTest(context);
        }

        const QVector<AbstractWidgetTest *> & WidgetTestManager::tests() const
        {
            return _tests;
        }

    } // namespace WidgetTest
} // namespace djv
