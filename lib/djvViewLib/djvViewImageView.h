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

//! \file djvViewImageView.h

#ifndef DJV_VIEW_IMAGE_VIEW_H
#define DJV_VIEW_IMAGE_VIEW_H

#include <djvViewUtil.h>

#include <djvImageView.h>

#include <djvFileInfo.h>

class  djvViewContext;
struct djvViewHudInfo;
struct djvViewImageViewPrivate;

//! \addtogroup djvViewImage
//@{

//------------------------------------------------------------------------------
//! \class djvViewImageView
//!
//! This class provides an image view widget.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewImageView : public djvImageView
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewImageView(djvViewContext *, QWidget * parent = 0);

    //! Destructor.

    virtual ~djvViewImageView();

    //! Get whether the mouse is inside the view.

    bool isMouseInside() const;

    //! Get the mouse position.

    const djvVector2i & mousePos() const;
    
    virtual QSize sizeHint() const;

    virtual QSize minimumSizeHint() const;

public Q_SLOTS:

    //! Set the zoom using the mouse pointer for focus.

    void setZoomFocus(double);

    //! Set the grid.

    void setGrid(djvViewUtil::GRID);

    //! Set the grid color.

    void setGridColor(const djvColor &);

    //! Set whether the HUD is enabled.

    void setHudEnabled(bool);

    //! Set the HUD information.

    void setHudInfo(const djvViewHudInfo &);

    //! Set the HUD color.

    void setHudColor(const djvColor &);

    //! Set the HUD background.

    void setHudBackground(djvViewUtil::HUD_BACKGROUND);

    //! Set the HUD background color.

    void setHudBackgroundColor(const djvColor &);

Q_SIGNALS:

    //! This signal is emitted when the view is picked.

    void pickChanged(const djvVector2i &);

    //! This signal is emitted when the mouse wheel is changed.

    void mouseWheelChanged(djvViewUtil::MOUSE_WHEEL);

    //! This signal is emitted when the mouse wheel value is changed.

    void mouseWheelValueChanged(int);

    //! This signal is emitted when a file is dropped on the view.

    void fileDropped(const djvFileInfo &);

protected:

    virtual void timerEvent(QTimerEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void paintGL();

private Q_SLOTS:

    void hudInfoCallback(const QVector<bool> &);

private:

    void drawGrid();

    void drawHud();
    void drawHud(
        const QStringList & upperLeft,
        const QStringList & lowerLeft,
        const QStringList & upperRight,
        const QStringList & lowerRight);
    djvBox2i drawHud(
        const QString &     string,
        const djvVector2i & position);
    QSize drawHudSize(const QString &) const;

    DJV_PRIVATE_COPY(djvViewImageView);
    
    djvViewImageViewPrivate * _p;
};

//@} // djvViewImage

#endif // DJV_VIEW_IMAGE_VIEW_H

