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

//! \file djvViewViewPrefs.h

#ifndef DJV_VIEW_VIEW_PREFS_H
#define DJV_VIEW_VIEW_PREFS_H

#include <djvViewAbstractPrefs.h>
#include <djvViewImageView.h>

#include <djvColor.h>
#include <djvVector.h>

//! \addtogroup djvViewView
//@{

//------------------------------------------------------------------------------
//! \class djvViewViewPrefs
//!
//! This class provides the view group preferences.
//------------------------------------------------------------------------------

class DJV_VIEW_LIB_EXPORT djvViewViewPrefs : public djvViewAbstractPrefs
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvViewViewPrefs(djvViewContext *, QObject * parent = 0);

    //! Destructor.

    virtual ~djvViewViewPrefs();
    
    //! Get the default mouse wheel zoom factor.
    
    static djvViewUtil::ZOOM_FACTOR zoomFactorDefault();
    
    //! Get the mouse wheel zoom factor.
    
    djvViewUtil::ZOOM_FACTOR zoomFactor() const;
    
    //! Get the default background color.
    
    static djvColor backgroundDefault();

    //! Get the background color.

    const djvColor & background() const;

    //! Get the default grid.

    static djvViewUtil::GRID gridDefault();

    //! Get the view grid.

    djvViewUtil::GRID grid() const;

    //! Get the default view grid color.
    
    static djvColor gridColorDefault();

    //! Get the view grid color.

    const djvColor & gridColor() const;

    //! Get the default for whether the HUD is enabled.
    
    static bool hudEnabledDefault();
    
    //! Get whether the HUD is enabled.

    bool isHudEnabled() const;

    //! Get the default HUD information.
    
    static QVector<bool> hudInfoDefault();

    //! Get the HUD information.

    QVector<bool> hudInfo() const;

    //! Get the HUD information.

    bool isHudInfo(djvViewUtil::HUD in) const;

    //! Get the default HUD color.
    
    static djvColor hudColorDefault();

    //! Get the HUD color.

    const djvColor & hudColor() const;

    //! Get the default HUD background.
    
    static djvViewUtil::HUD_BACKGROUND hudBackgroundDefault();

    //! Get the HUD background.

    djvViewUtil::HUD_BACKGROUND hudBackground() const;

    //! Get the default HUD background color.
    
    static djvColor hudBackgroundColorDefault();

    //! Get the HUD background color.

    const djvColor & hudBackgroundColor() const;

public Q_SLOTS:

    //! Set the mouse wheel zoom factor.
    
    void setZoomFactor(djvViewUtil::ZOOM_FACTOR);

    //! Set the background color.

    void setBackground(const djvColor &);

    //! Set the view grid.

    void setGrid(djvViewUtil::GRID);

    //! Set the view grid color.

    void setGridColor(const djvColor &);
    
    //! Set whether the HUD is enabled.

    void setHudEnabled(bool);

    //! Set the HUD informati0on.

    void setHudInfo(const QVector<bool> &);

    //! Set the HUD information.

    void setHudInfo(djvViewUtil::HUD, bool);
    
    //! Set the HUD color.

    void setHudColor(const djvColor &);
    
    //! Set the HUD background.

    void setHudBackground(djvViewUtil::HUD_BACKGROUND);
    
    //! Set the HUD background color.

    void setHudBackgroundColor(const djvColor &);

Q_SIGNALS:

    //! This signal is emitted when the mouse wheel zoom factor is changed.
    
    void zoomFactorChanged(djvViewUtil::ZOOM_FACTOR);

    //! This signal is emitted when the background color is changed.
    
    void backgroundChanged(const djvColor &);

    //! This signal is emitted when the view grid is changed.

    void gridChanged(djvViewUtil::GRID);
    
    //! This signal is emitted when the view grid color is changed.
    
    void gridColorChanged(const djvColor &);

    //! This signal is emitted when the HUD is enabled or disabled.

    void hudEnabledChanged(bool);
    
    //! This signal is emitted when the HUD information is changed.
    
    void hudInfoChanged(const QVector<bool> &);
    
    //! This signal is emitted when the HUD color is changed.
    
    void hudColorChanged(const djvColor &);
    
    //! This signal is emitted when the HUD background is changed.
    
    void hudBackgroundChanged(djvViewUtil::HUD_BACKGROUND);
    
    //! This signal is emitted when the HUD background color is changed.
    
    void hudBackgroundColorChanged(const djvColor &);

    //! This signal is emitted when a preference is changed.

    void prefChanged();
    
private:

    djvViewUtil::ZOOM_FACTOR    _zoomFactor;
    djvColor                    _background;
    djvViewUtil::GRID           _grid;
    djvColor                    _gridColor;
    bool                        _hudEnabled;
    QVector<bool>               _hudInfo;
    djvColor                    _hudColor;
    djvViewUtil::HUD_BACKGROUND _hudBackground;
    djvColor                    _hudBackgroundColor;
};

//@} // djvViewView

#endif // DJV_VIEW_VIEW_PREFS_H

