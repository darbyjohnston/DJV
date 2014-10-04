//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvViewWindowPrefs.h

#ifndef DJV_VIEW_WINDOW_PREFS_H
#define DJV_VIEW_WINDOW_PREFS_H

#include <djvView.h>
#include <djvViewAbstractPrefs.h>

#include <djvBox.h>

//! \addtogroup djvViewWindow
//@{

//------------------------------------------------------------------------------
//! \class djvViewWindowPrefs
//!
//! This class provides the window group preferences.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewWindowPrefs : public djvViewAbstractPrefs
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewWindowPrefs(QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewWindowPrefs();
    
    //! Get the default for whether resize fit is enabled.
    
    static bool resizeFitDefault();

    //! Get whether resize fit is enabled.

    bool hasResizeFit() const;

    //! Get the default maximum resize amount.
    
    static djvView::WINDOW_RESIZE_MAX resizeMaxDefault();

    //! Get the maximum resize amount.

    djvView::WINDOW_RESIZE_MAX resizeMax() const;

    //! Get the default tool bar visiblity.
    
    static QVector<bool> toolBarDefault();
    
    //! Get the tool bar visibility.

    const QVector<bool> & toolBar() const;

    //! Get the global preferences.

    static djvViewWindowPrefs * global();

public Q_SLOTS:

    //! Set whether resize fit is enabled.

    void setResizeFit(bool);

    //! Set the maximum resize amount.

    void setResizeMax(djvView::WINDOW_RESIZE_MAX);

    //! Set the tool bar visibility.

    void setToolBar(const QVector<bool> &);

Q_SIGNALS:

    //! This signal is emitted when resize fit is changed.

    void resizeFitChanged(bool);

    //! This signal is emitted when the maximum resize amount is changed.

    void resizeMaxChanged(djvView::WINDOW_RESIZE_MAX);

    //! This signal is emitted when the tool bar visibility is changed.

    void toolBarChanged(const QVector<bool> &);

private:

    djvBox2i                   _geom;
    bool                       _resizeFit;
    djvView::WINDOW_RESIZE_MAX _resizeMax;
    QVector<bool>              _toolBar;
};

//@} // djvViewWindow

#endif // DJV_VIEW_WINDOW_PREFS_H

