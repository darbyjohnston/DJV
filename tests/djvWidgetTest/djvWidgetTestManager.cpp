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

//! \file djvWidgetTestManager.cpp

#include <djvWidgetTestManager.h>

#include <djvAboutDialogTest.h>
#include <djvChoiceDialogTest.h>
#include <djvColorWidgetTest.h>
#include <djvColorDialogTest.h>
#include <djvFileBrowserTest.h>
#include <djvFileEditTest.h>
#include <djvIconLibraryTest.h>
#include <djvInfoDialogTest.h>
#include <djvInputDialogTest.h>
#include <djvIntSliderTest.h>
#include <djvMessagesDialogTest.h>
#include <djvMultiChoiceDialogTest.h>
#include <djvNoticeDialogTest.h>
#include <djvNumWidgetTest.h>
#include <djvPixelWidgetTest.h>
#include <djvPrefsDialogTest.h>
#include <djvProgressDialogTest.h>
#include <djvQuestionDialogTest.h>
#include <djvSearchBoxTest.h>
#include <djvShortcutsWidgetTest.h>
#include <djvToolButtonTest.h>
#include <djvVectorWidgetTest.h>
#include <djvViewWidgetTest.h>

#include <djvGuiContext.h>

//------------------------------------------------------------------------------
// djvWidgetTestManager
//------------------------------------------------------------------------------

djvWidgetTestManager::djvWidgetTestManager(djvGuiContext * context)
{
    _tests += new djvAboutDialogTest(context);
    _tests += new djvChoiceDialogTest(context);
    _tests += new djvColorWidgetTest(context);
    _tests += new djvColorDialogTest(context);
    _tests += new djvFileBrowserTest(context);
    _tests += new djvFileEditTest(context);
    _tests += new djvIconLibraryTest(context);
    _tests += new djvInfoDialogTest(context);
    _tests += new djvInputDialogTest(context);
    _tests += new djvIntSliderTest(context);
    _tests += new djvMessagesDialogTest(context);
    _tests += new djvMultiChoiceDialogTest(context);
    _tests += new djvNoticeDialogTest(context);
    _tests += new djvNumWidgetTest(context);
    _tests += new djvPixelWidgetTest(context);
    _tests += new djvPrefsDialogTest(context);
    _tests += new djvProgressDialogTest(context);
    _tests += new djvQuestionDialogTest(context);
    _tests += new djvSearchBoxTest(context);
    _tests += new djvShortcutsWidgetTest(context);
    _tests += new djvToolButtonTest(context);
    _tests += new djvVectorWidgetTest(context);
    _tests += new djvViewWidgetTest(context);
}
    
const QVector<djvAbstractWidgetTest *> & djvWidgetTestManager::tests() const
{
    return _tests;
}
