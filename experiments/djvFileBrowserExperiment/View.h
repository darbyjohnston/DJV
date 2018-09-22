//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#pragma once

#include <ActionData.h>

#include <djvUtil.h>

#include <QWidget>

#include <memory>

class Model;

class djvUIContext;

//------------------------------------------------------------------------------
//! \class View
//!
//! This class provides the file browser view.
//------------------------------------------------------------------------------

class View : public QWidget
{
    Q_OBJECT
    Q_ENUMS(MODE)
    
    //! This property holds the view mode.
    Q_PROPERTY(
        MODE   mode
        READ   mode
        WRITE  setMode
        NOTIFY modeChanged)
        
public:

    //! This enumeration provides the view mode.
    enum MODE
    {
        DETAILS,
        CONTACT_SHEET,

        MODE_COUNT
    };

    //! Get the view mode data.
    static const ActionDataList & modeData();

    View(Model *        model,
         djvUIContext * context,
         QWidget *      parent  = nullptr);
    
    virtual ~View();
    
    //! Get the current view mode.
    MODE mode() const;
    
    virtual QSize sizeHint() const;
    
public Q_SLOTS:
    //! Set the current view mode.
    void setMode(View::MODE);
    
Q_SIGNALS:
    //! This signal is emitted when the view mode is changed.
    void modeChanged(View::MODE);

protected:
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    virtual void resizeEvent(QResizeEvent *);

private Q_SLOTS:
    void startCallback();
    void thumbnailSizeCallback(int);

private:
    void updateWidget();
    
    DJV_PRIVATE_COPY(View)
    
    struct Private;
    std::unique_ptr<Private> _p;
};

