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

//! \file djvViewViewPrefsWidget.h

#ifndef DJV_VIEW_VIEW_PREFS_WIDGET_H
#define DJV_VIEW_VIEW_PREFS_WIDGET_H

#include <djvViewAbstractPrefsWidget.h>

#include <djvColor.h>

struct djvViewViewPrefsWidgetPrivate;

class QListWidgetItem;

//! \addtogroup djvViewView
//@{

//------------------------------------------------------------------------------
//! \class djvViewViewPrefsWidget
//!
//! This class provides the view group preferences widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewViewPrefsWidget :
    public djvViewAbstractPrefsWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewViewPrefsWidget(djvViewContext *);

    //! Destructor.

    virtual ~djvViewViewPrefsWidget();
    
    virtual void resetPreferences();

private Q_SLOTS:

    void zoomFactorCallback(int);
    void backgroundCallback(const djvColor &);
    void gridCallback(int);
    void gridColorCallback(const djvColor &);
    void hudEnabledCallback(bool);
    void hudInfoCallback(QListWidgetItem *);
    void hudColorCallback(const djvColor &);
    void hudBackgroundCallback(int);
    void hudBackgroundColorCallback(const djvColor &);

    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewViewPrefsWidget);
    
    djvViewViewPrefsWidgetPrivate * _p;
};

//@} // djvViewView

#endif // DJV_VIEW_VIEW_PREFS_H

