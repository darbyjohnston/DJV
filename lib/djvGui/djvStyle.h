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

//! \file djvStyle.h

#ifndef DJV_STYLE_H
#define DJV_STYLE_H

#include <djvGuiExport.h>

#include <djvColor.h>
#include <djvUtil.h>

#include <QObject>
#include <QFont>

struct djvStylePrivate;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvStyle
//!
//! This class provides the user interface style.
//!
//! \todo Can we use events instead of signals for notifying other objects
//! when the style changes?
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvStyle : public QObject
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvStyle(QObject * parent = 0);

    //! Destructor.

    ~djvStyle();

    //! This struct provides a color palette.

    struct DJV_GUI_EXPORT Palette
    {
        //! Constructor.

        explicit Palette(
            const QString &  name        = QString(),
            const djvColor & foreground  = djvColor(),
            const djvColor & background  = djvColor(),
            const djvColor & background2 = djvColor(),
            const djvColor & button      = djvColor(),
            const djvColor & select      = djvColor());
        
        QString  name;
        djvColor foreground;
        djvColor background;
        djvColor background2;
        djvColor button;
        djvColor select;
    };
    
    //! Get the default color palettes.
    
    static const QVector<Palette> & palettesDefault();
    
    //! Get the list of color palettes.
    
    const QVector<Palette> & palettes();

    //! Set the list of color palettes.
    
    void setPalettes(const QVector<Palette> &);

    //! Get the default palette index.
    
    static int palettesIndexDefault();
    
    //! Get the current color palette index.
    
    int palettesIndex() const;
    
    //! Set the current color palette index.
    
    void setPalettesIndex(int);
    
    //! Get the current color palette.
    
    const Palette & palette() const;
    
    //! Set the current color palette.
    
    void setPalette(const Palette &);
    
    //! Get the list of color palette names.
    
    QStringList paletteNames() const;

    //! Get the default color swatch transparency.

    static bool colorSwatchTransparencyDefault();

    //! Get whether color swatches show transparency.

    bool hasColorSwatchTransparency() const;

    //! Set whether color swatches show transparency.

    void setColorSwatchTransparency(bool);

    //! This struct provides size metrics.
    
    struct DJV_GUI_EXPORT SizeMetric
    {
        //! Constructor.
        
        explicit SizeMetric(const QString & name = QString(), int fontSize = 0);
    
        QString name;
        int     fontSize;
        int     pickSize;
        int     handleSize;
        int     spacing;
        int     largeSpacing;
        int     margin;
        int     textMargin;
        int     widgetMargin;
        int     largeMargin;
        int     iconSize;
        int     toolIconSize;
        int     buttonSize;
        int     textSize;
        int     swatchSize;
        int     thumbnailSize;
    };
    
    //! Get the default size metrics.
    
    static const QVector<SizeMetric> & sizeMetricsDefault();
    
    //! Get the list of size metrics.
    
    const QVector<SizeMetric> & sizeMetrics();
    
    //! Set the list of size metrics.
    
    void setSizeMetrics(const QVector<SizeMetric> &);
    
    //! Get the current size metric.
    
    const SizeMetric & sizeMetric() const;
    
    //! Set the current size metric.
    
    void setSizeMetric(const SizeMetric &);

    //! Get the default size metric index.
    
    static int sizeMetricsIndexDefault();
    
    //! Get the current size metric index.
    
    int sizeMetricsIndex() const;
    
    //! Set the current size metric index.
    
    void setSizeMetricsIndex(int);
    
    //! Get the list of size metric names.
    
    QStringList sizeMetricNames() const;

    //! This struct provides the fonts.
    
    struct DJV_GUI_EXPORT Fonts
    {
        Fonts();
        
        QFont normal;
        QFont fixed;
    };
    
    //! Get the default fonts.
    
    static const Fonts & fontsDefault();

    //! Get the fonts.

    const Fonts & fonts() const;

    //! Set the fonts.
    
    void setFonts(const Fonts &);

Q_SIGNALS:

    //! This signal is emitted when color swatch transparency is changed.

    void colorSwatchTransparencyChanged(bool);

    //! This signal is emitted when the size metrics are changed.
    
    void sizeMetricsChanged();
    
    //! This signal is emitted when the fonts are changed.
    
    void fontsChanged();

private:

    void colorUpdate();
    void sizeUpdate();
    void fontsUpdate();
    
    DJV_PRIVATE_COPY(djvStyle);
    
    djvStylePrivate * _p;
};

//------------------------------------------------------------------------------

DJV_COMPARISON_OPERATOR(DJV_GUI_EXPORT, djvStyle::Palette);
DJV_COMPARISON_OPERATOR(DJV_GUI_EXPORT, djvStyle::SizeMetric);
DJV_COMPARISON_OPERATOR(DJV_GUI_EXPORT, djvStyle::Fonts);

DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvStyle::Palette);
DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvStyle::SizeMetric);
DJV_STRING_OPERATOR(DJV_GUI_EXPORT, djvStyle::Fonts);

//@} // djvGuiMisc

#endif // DJV_STYLE_H

