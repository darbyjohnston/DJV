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

//! \file djvImagePrefsWidget.h

#ifndef DJV_IMAGE_PREFS_WIDGET_H
#define DJV_IMAGE_PREFS_WIDGET_H

#include <djvAbstractPrefsWidget.h>

#include <djvUtil.h>

struct djvImagePrefsWidgetPrivate;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvImagePrefsWidget
//!
//! This class provides an image preferences widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvImagePrefsWidget : public djvAbstractPrefsWidget
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvImagePrefsWidget(djvGuiContext * context, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvImagePrefsWidget();

    virtual void resetPreferences();

    private Q_SLOTS:

    void filterMinCallback(int);
    void filterMagCallback(int);

private:

    void widgetUpdate();

    DJV_PRIVATE_COPY(djvImagePrefsWidget);
    
    djvImagePrefsWidgetPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_IMAGE_PREFS_WIDGET_H

