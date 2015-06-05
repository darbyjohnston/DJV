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

//! \file djvStylePrefsWidget.h

#ifndef DJV_STYLE_PREFS_WIDGET_H
#define DJV_STYLE_PREFS_WIDGET_H

#include <djvAbstractPrefsWidget.h>

#include <djvUtil.h>

struct djvStylePrefsWidgetPrivate;

class djvColor;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvStylePrefsWidget
//!
//! This class provides a style preferences widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvStylePrefsWidget : public djvAbstractPrefsWidget
{
    Q_OBJECT

public:

    //! Constructor.

    explicit djvStylePrefsWidget(djvGuiContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvStylePrefsWidget();

    virtual void resetPreferences();

    private Q_SLOTS:

    void colorCallback(int);
    void colorForegroundCallback(const djvColor &);
    void colorBackgroundCallback(const djvColor &);
    void colorBackground2Callback(const djvColor &);
    void colorButtonCallback(const djvColor &);
    void colorSelectCallback(const djvColor &);
    void colorSwatchTransparencyCallback(bool);
    void sizeCallback(int);
    void sizeValueCallback(int);
    void fontNormalCallback(const QFont &);
    void fontFixedCallback(const QFont &);

private:

    void widgetUpdate();

    DJV_PRIVATE_COPY(djvStylePrefsWidget);
    
    djvStylePrefsWidgetPrivate * _p;
};

//@} // djvGuiMisc

#endif // DJV_STYLE_PREFS_WIDGET_H

