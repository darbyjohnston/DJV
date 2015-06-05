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

//! \file djvViewDisplayProfileWidget.h

#ifndef DJV_VIEW_DISPLAY_PROFILE_WIDGET_H
#define DJV_VIEW_DISPLAY_PROFILE_WIDGET_H

#include <djvViewDisplayProfile.h>

#include <djvUtil.h>

#include <QWidget>

class  djvViewContext;
struct djvViewDisplayProfileWidgetPrivate;
class  djvViewImageView;

//! \addtogroup djvViewImage
//@{

//------------------------------------------------------------------------------
//! \class djvViewDisplayProfileWidget
//!
//! This class provides a display profile widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewDisplayProfileWidget : public QWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewDisplayProfileWidget(
        const djvViewImageView * viewWidget,
        djvViewContext *         context,
        QWidget *                parent     = 0);

    //! Destructor.

    virtual ~djvViewDisplayProfileWidget();
    
    //! Get the display profile.
    
    const djvViewDisplayProfile & displayProfile() const;
    
public Q_SLOTS:

    //! Set the display profile.
    
    void setDisplayProfile(const djvViewDisplayProfile &);

Q_SIGNALS:

    //! This signal is emitted when the display profile is changed.

    void displayProfileChanged(const djvViewDisplayProfile &);

private Q_SLOTS:

    void lutCallback(const djvFileInfo &);
    void brightnessCallback(double);
    void contrastCallback(double);
    void saturationCallback(double);
    void levelsIn0Callback(double);
    void levelsIn1Callback(double);
    void gammaCallback(double);
    void levelsOut0Callback(double);
    void levelsOut1Callback(double);
    void softClipCallback(double);
    void addCallback();
    void resetCallback();

private:

    void widgetUpdate();
    
    DJV_PRIVATE_COPY(djvViewDisplayProfileWidget);
    
    djvViewDisplayProfileWidgetPrivate * _p;
};

//@} // djvViewImage

#endif // DJV_VIEW_DISPLAY_PROFILE_WIDGET_H

