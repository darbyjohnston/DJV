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

//! \file djvColorSwatch.h

#ifndef DJV_COLOR_SWATCH_H
#define DJV_COLOR_SWATCH_H

#include <djvGuiExport.h>

#include <djvColor.h>

#include <QWidget>

class djvGuiContext;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvColorSwatch
//!
//! This class provides a basic color swatch widget.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvColorSwatch : public QWidget
{
    Q_OBJECT
    Q_ENUMS(SWATCH_SIZE)
    
    //! This property holds the color.
    
    Q_PROPERTY(
        djvColor color
        READ     color
        WRITE    setColor
        NOTIFY   colorChanged)
    
public:

    //! Constructor.

    explicit djvColorSwatch(djvGuiContext *, QWidget * parent = 0);

    //! Get the color.

    const djvColor & color() const;
    
    //! This enumeration provides the swatch size.
    
    enum SWATCH_SIZE
    {
        SWATCH_SMALL,
        SWATCH_MEDIUM,
        SWATCH_LARGE,
    };
    
    //! Get the size.
    
    SWATCH_SIZE swatchSize() const;
    
    //! Set the swatch size.
    
    void setSwatchSize(SWATCH_SIZE);
    
    //! Get whether the color dialog is enabled.
    
    bool isColorDialogEnabled() const;
    
    //! Set whether the color dialog is enabled.
    
    void setColorDialogEnabled(bool);
    
    virtual QSize sizeHint() const;

public Q_SLOTS:

    //! Set the color.

    void setColor(const djvColor &);

Q_SIGNALS:

    //! This signal is emitted when the color is changed.

    void colorChanged(const djvColor &);

    //! This signal is emitted when the swatch is clicked.

    void clicked();
    
protected:

    virtual void mousePressEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private Q_SLOTS:

    void sizeMetricsCallback();

private:

    djvColor        _color;
    SWATCH_SIZE     _swatchSize;
    bool            _colorDialogEnabled;
    djvGuiContext * _context;
};

//@} // djvGuiWidget

#endif // DJV_COLOR_SWATCH_H

