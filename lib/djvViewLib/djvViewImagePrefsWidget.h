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

//! \file djvViewImagePrefsWidget.h

#ifndef DJV_VIEW_IMAGE_PREFS_WIDGET_H
#define DJV_VIEW_IMAGE_PREFS_WIDGET_H

#include <djvViewAbstractPrefsWidget.h>

struct djvViewImagePrefsWidgetPrivate;

class QListWidgetItem;

//! \addtogroup djvViewImage
//@{

//------------------------------------------------------------------------------
//! \class djvViewImagePrefsWidget
//!
//! This class provides the image group preferences widget.
//------------------------------------------------------------------------------

class djvViewImagePrefsWidget : public djvViewAbstractPrefsWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvViewImagePrefsWidget(djvViewContext *);

    //! Destructor.

    virtual ~djvViewImagePrefsWidget();
    
    virtual void resetPreferences();

private Q_SLOTS:

    void frameStoreFileReloadCallback(bool);
    void mirrorHCallback(bool);
    void mirrorVCallback(bool);
    void scaleCallback(int);
    void rotateCallback(int);
    void colorProfileCallback(bool);
    void displayProfileCallback(int);
    void displayProfileCallback(QListWidgetItem *);
    void addDisplayProfileCallback();
    void removeDisplayProfileCallback();
    void moveDisplayProfileUpCallback();
    void moveDisplayProfileDownCallback();
    void channelCallback(int);
    
    void widgetUpdate();

private:

    DJV_PRIVATE_COPY(djvViewImagePrefsWidget);
    
    djvViewImagePrefsWidgetPrivate * _p;
};

//@} // djvViewImage

#endif // DJV_VIEW_IMAGE_PREFS_WIDGET_H

