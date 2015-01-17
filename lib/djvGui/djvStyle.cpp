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

//! \file djvStyle.cpp

#include <djvStyle.h>

#include <djvPrefs.h>

#include <djvColorUtil.h>

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>

//------------------------------------------------------------------------------
// djvStyle::Palette
//------------------------------------------------------------------------------

djvStyle::Palette::Palette(
    const QString &  name,
    const djvColor & foreground,
    const djvColor & background,
    const djvColor & background2,
    const djvColor & button,
    const djvColor & select) :
    name       (name),
    foreground (foreground),
    background (background),
    background2(background2),
    button     (button),
    select     (select)
{}

//------------------------------------------------------------------------------
// djvStyle::SizeMetric
//------------------------------------------------------------------------------

djvStyle::SizeMetric::SizeMetric(const QString & name, int fontSize) :
    name         (name),
    fontSize     (fontSize),
    pickSize     (static_cast<int>(fontSize * 0.8)),
    handleSize   (static_cast<int>(fontSize * 3.0)),
    spacing      (4),
    largeSpacing (16),
    margin       (4),
    textMargin   (2),
    widgetMargin (4),
    largeMargin  (16),
    iconSize     (22),
    toolIconSize (11),
    buttonSize   (static_cast<int>(fontSize * 4)),
    textSize     (static_cast<int>(fontSize * 10)),
    swatchSize   (static_cast<int>(fontSize * 4)),
    thumbnailSize(static_cast<int>(fontSize * 10))
{}

//------------------------------------------------------------------------------
// djvStyle::Fonts
//------------------------------------------------------------------------------

djvStyle::Fonts::Fonts() :
    normal(qApp->font()),
    fixed ("Monospace")
{
#if QT_VERSION >= 0x040800
    fixed.setStyleHint(QFont::Monospace);
#endif // QT_VERSION
}

//------------------------------------------------------------------------------
// djvStyle::P
//------------------------------------------------------------------------------

struct djvStyle::P
{
    P() :
        palettes               (palettesDefault()),
        palettesIndex          (palettesIndexDefault()),
        colorSwatchTransparency(colorSwatchTransparencyDefault()),
        sizeMetrics            (sizeMetricsDefault()),
        sizeMetricsIndex       (sizeMetricsIndexDefault())
    {}

    QVector<Palette>    palettes;
    int                 palettesIndex;
    bool                colorSwatchTransparency;
    QVector<SizeMetric> sizeMetrics;
    int                 sizeMetricsIndex;
    Fonts               fonts;
};

//------------------------------------------------------------------------------
// djvStyle
//------------------------------------------------------------------------------

djvStyle::djvStyle(QObject * parent) :
    QObject(parent),
    _p(new P)
{
    //DJV_DEBUG("djvStyle::djvStyle");
    
    // Load preferences.
    
    djvPrefs prefs("djvStyle", djvPrefs::SYSTEM);
    
    int paletteCount = 0;
    
    prefs.get("paletteCount", paletteCount);
    
    //DJV_DEBUG_PRINT("paletteCount = " << paletteCount);
    
    for (int i = 0; i < paletteCount; ++i)
    {
        Palette palette;
        prefs.get(QString("palette%1").arg(i), palette);
        
        //DJV_DEBUG_PRINT("palette = " << palette.name);
        
        int j = 0;
        
        for (; j < _p->palettes.count(); ++j)
        {
            if (palette.name == _p->palettes[j].name)
            {
                _p->palettes[j] = palette;
                
                break;
            }
        }
        
        if (j == _p->palettes.count())
        {
            _p->palettes += palette;
        }
    }
    
    prefs.get("palettesIndex", _p->palettesIndex);
    
    //DJV_DEBUG_PRINT("palettesIndex = " << _p->palettesIndex);

    int sizeMetricsCount = 0;
    
    prefs.get("sizeMetricsCount", sizeMetricsCount);
    
    for (int i = 0; i < sizeMetricsCount; ++i)
    {
        SizeMetric sizeMetric;
        
        prefs.get(QString("sizeMetric%1").arg(i), sizeMetric);
        
        int j = 0;
        
        for (; j < _p->sizeMetrics.count(); ++j)
        {
            if (sizeMetric.name == _p->sizeMetrics[j].name)
            {
                _p->sizeMetrics[j] = sizeMetric;
                
                break;
            }
        }
        
        if (j == _p->sizeMetrics.count())
        {
            _p->sizeMetrics += sizeMetric;
        }
    }
    
    prefs.get("sizeMetricsIndex", _p->sizeMetricsIndex);
    
    prefs.get("fonts", _p->fonts);
    
    //DJV_DEBUG_PRINT("sizeMetricsIndex = " << _p->sizeMetricsIndex);

    // Initialize.
    
    colorUpdate();
    sizeUpdate();
    fontsUpdate();
}

djvStyle::~djvStyle()
{
    //DJV_DEBUG("djvStyle::~djvStyle");

    // Save preferences.
    
    djvPrefs prefs("djvStyle", djvPrefs::SYSTEM);
    
    const int paletteCount = _p->palettes.count();
    
    prefs.set("paletteCount", paletteCount);
    
    for (int i = 0; i < paletteCount; ++i)
    {
        prefs.set(QString("palette%1").arg(i), _p->palettes[i]);
    }
    
    //DJV_DEBUG_PRINT("palettesIndex = " << _p->palettesIndex);
    
    prefs.set("palettesIndex", _p->palettesIndex);

    const int sizeMetricsCount = _p->sizeMetrics.count();
    
    prefs.set("sizeMetricsCount", sizeMetricsCount);
    
    for (int i = 0; i < sizeMetricsCount; ++i)
    {
        prefs.set(QString("sizeMetric%1").arg(i), _p->sizeMetrics[i]);
    }

    prefs.set("sizeMetricsIndex", _p->sizeMetricsIndex);

    prefs.set("fonts", _p->fonts);

    delete _p;
}
    
const QVector<djvStyle::Palette> & djvStyle::palettesDefault()
{
    static const QVector<djvStyle::Palette> data = QVector<djvStyle::Palette>() <<
        Palette(
            tr("Dark"),
            djvColor(0.9f),
            djvColor(0.25f),
            djvColor(0.15f),
            djvColor(0.25f),
            djvColor(0.7f, 0.6f, 0.3f)) <<
        Palette(
            tr("Light"),
            djvColor(0.1f),
            djvColor(0.8f),
            djvColor(0.7f),
            djvColor(0.7f),
            djvColor(0.7f, 0.6f, 0.3f)) <<
        Palette(
            tr("Default"),
            djvColorUtil::fromQt(qApp->palette().color(QPalette::Foreground)),
            djvColorUtil::fromQt(qApp->palette().color(QPalette::Background)),
            djvColorUtil::fromQt(qApp->palette().color(QPalette::Base)),
            djvColorUtil::fromQt(qApp->palette().color(QPalette::Button)),
            djvColorUtil::fromQt(qApp->palette().color(QPalette::Highlight))) <<
        Palette(
            tr("Custom"),
            djvColor(0.9f),
            djvColor(0.0f, 0.3f, 0.0f),
            djvColor(0.3f, 0.4f, 0.3f),
            djvColor(0.0f, 0.4f, 0.0f),
            djvColor(0.7f, 0.9f, 0.7f));

    return data;
}

const QVector<djvStyle::Palette> & djvStyle::palettes()
{
    return _p->palettes;
}

void djvStyle::setPalettes(const QVector<Palette> & palettes)
{
    if (palettes == _p->palettes)
        return;
    
    _p->palettes = palettes;
    
    colorUpdate();
}

int djvStyle::palettesIndexDefault()
{
    return 0;
}

int djvStyle::palettesIndex() const
{
    return _p->palettesIndex;
}

void djvStyle::setPalettesIndex(int index)
{
    if (index == _p->palettesIndex)
        return;
    
    _p->palettesIndex = index;
    
    colorUpdate();
}

const djvStyle::Palette & djvStyle::palette() const
{
    return _p->palettes[_p->palettesIndex];
}

void djvStyle::setPalette(const Palette & palette)
{
    if (palette == _p->palettes[_p->palettesIndex])
        return;
    
    _p->palettes[_p->palettesIndex] = palette;
    
    colorUpdate();
}

QStringList djvStyle::paletteNames() const
{
    QStringList names;
    
    for (int i = 0; i < _p->palettes.count(); ++i)
    {
        names += _p->palettes[i].name;
    }
    
    return names;
}

bool djvStyle::colorSwatchTransparencyDefault()
{
    return true;
}

bool djvStyle::hasColorSwatchTransparency() const
{
    return _p->colorSwatchTransparency;
}

void djvStyle::setColorSwatchTransparency(bool in)
{
    if (in == _p->colorSwatchTransparency)
        return;

    _p->colorSwatchTransparency = in;

    Q_EMIT colorSwatchTransparencyChanged(_p->colorSwatchTransparency);
}

const QVector<djvStyle::SizeMetric> & djvStyle::sizeMetricsDefault()
{
    static const QVector<SizeMetric> data = QVector<SizeMetric>() <<
        SizeMetric(tr("Small"), 9) <<
        SizeMetric(tr("Medium"), 12) <<
        SizeMetric(tr("Large"), 16) <<
        SizeMetric(tr("Custom"), 24);
    
    return data;
}

const QVector<djvStyle::SizeMetric> & djvStyle::sizeMetrics()
{
    return _p->sizeMetrics;
}

void djvStyle::setSizeMetrics(const QVector<SizeMetric> & sizeMetrics)
{
    if (sizeMetrics == _p->sizeMetrics)
        return;
    
    _p->sizeMetrics = sizeMetrics;
    
    sizeUpdate();

    Q_EMIT sizeMetricsChanged();
}

int djvStyle::sizeMetricsIndexDefault()
{
    return 1;
}

int djvStyle::sizeMetricsIndex() const
{
    return _p->sizeMetricsIndex;
}

void djvStyle::setSizeMetricsIndex(int index)
{
    if (index == _p->sizeMetricsIndex)
        return;
    
    _p->sizeMetricsIndex = index;
    
    sizeUpdate();
    
    Q_EMIT sizeMetricsChanged();
}

const djvStyle::SizeMetric & djvStyle::sizeMetric() const
{
    return _p->sizeMetrics[_p->sizeMetricsIndex];
}

void djvStyle::setSizeMetric(const SizeMetric & sizeMetric)
{
    if (sizeMetric == _p->sizeMetrics[_p->sizeMetricsIndex])
        return;
    
    _p->sizeMetrics[_p->sizeMetricsIndex] = sizeMetric;
    
    sizeUpdate();
    
    Q_EMIT sizeMetricsChanged();
}

QStringList djvStyle::sizeMetricNames() const
{
    QStringList names;
    
    for (int i = 0; i < _p->sizeMetrics.count(); ++i)
    {
        names += _p->sizeMetrics[i].name;
    }
    
    return names;
}

const djvStyle::Fonts & djvStyle::fontsDefault()
{
    static const djvStyle::Fonts data;
    
    return data;
}

const djvStyle::Fonts & djvStyle::fonts() const
{
    return _p->fonts;
}

void djvStyle::setFonts(const Fonts & fonts)
{
    if (fonts == _p->fonts)
        return;
    
    _p->fonts = fonts;
    
    fontsUpdate();
    
    Q_EMIT fontsChanged();
}

namespace
{

QColor toQColor(const djvColor & color)
{
    djvColor tmp(djvPixel::RGBA_U8);
    djvColorUtil::convert(color, tmp);
    
    return QColor(tmp.u8(0), tmp.u8(1), tmp.u8(2), tmp.u8(3));
}

} // namespace

void djvStyle::colorUpdate()
{
    //DJV_DEBUG("djvStyle::colorUpdate");
    //DJV_DEBUG_PRINT("foreground = " <<
    //    _p->palettes[_p->palettesIndex].foreground);
    //DJV_DEBUG_PRINT("background = " <<
    //    _p->palettes[_p->palettesIndex].background);
    //DJV_DEBUG_PRINT("background2 = " <<
    //    _p->palettes[_p->palettesIndex].background2);
    
    QPalette palette;
    
    palette.setColor(
        QPalette::Window,
        toQColor(_p->palettes[_p->palettesIndex].background));
    palette.setColor(
        QPalette::WindowText,
        toQColor(_p->palettes[_p->palettesIndex].foreground));
    palette.setColor(
        QPalette::Base,
        toQColor(_p->palettes[_p->palettesIndex].background2));
    palette.setColor(
        QPalette::AlternateBase,
        toQColor(_p->palettes[_p->palettesIndex].background2).darker(120));
    palette.setColor(
        QPalette::Text,
        toQColor(_p->palettes[_p->palettesIndex].foreground));
    palette.setColor(
        QPalette::Button,
        toQColor(_p->palettes[_p->palettesIndex].button));
    palette.setColor(
        QPalette::ButtonText,
        toQColor(_p->palettes[_p->palettesIndex].foreground));
    palette.setColor(
        QPalette::Highlight,
        toQColor(_p->palettes[_p->palettesIndex].select));
    
    qApp->setPalette(palette);
}

djvStyle * djvStyle::global()
{
    static djvStyle * global = 0;
    
    if (! global)
    {
        global = new djvStyle(qApp);
    }
    
    return global;
}

void djvStyle::sizeUpdate()
{
    //DJV_DEBUG("djvStyle::sizeUpdate");
    
    QFont font = _p->fonts.normal;
    font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
    qApp->setFont(font);
}

void djvStyle::fontsUpdate()
{
    //DJV_DEBUG("djvStyle::fontsUpdate");
    
    QFont font = _p->fonts.normal;
    font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
    qApp->setFont(font);
}

//------------------------------------------------------------------------------

bool operator == (const djvStyle::Palette & a, const djvStyle::Palette & b)
{
    return
        a.foreground  == b.foreground  &&
        a.background  == b.background  &&
        a.background2 == b.background2 &&
        a.button      == b.button      &&
        a.select      == b.select;
}

bool operator != (const djvStyle::Palette & a, const djvStyle::Palette & b)
{
    return ! (a == b);
}

bool operator == (const djvStyle::SizeMetric & a, const djvStyle::SizeMetric & b)
{
    return
        a.fontSize      == b.fontSize     &&
        a.pickSize      == b.pickSize     &&
        a.handleSize    == b.handleSize   &&
        a.spacing       == b.spacing      &&
        a.largeSpacing  == b.largeSpacing &&
        a.margin        == b.margin       &&
        a.textMargin    == b.textMargin   &&
        a.widgetMargin  == b.widgetMargin &&
        a.largeMargin   == b.largeMargin  &&
        a.iconSize      == b.iconSize     &&
        a.toolIconSize  == b.toolIconSize &&
        a.buttonSize    == b.buttonSize   &&
        a.textSize      == b.textSize     &&
        a.swatchSize    == b.swatchSize   &&
        a.thumbnailSize == b.thumbnailSize;
}

bool operator != (const djvStyle::SizeMetric & a, const djvStyle::SizeMetric & b)
{
    return ! (a == b);
}

bool operator == (const djvStyle::Fonts & a, const djvStyle::Fonts & b)
{
    return
        a.normal == b.normal &&
        a.fixed  == b.fixed;
}

bool operator != (const djvStyle::Fonts & a, const djvStyle::Fonts & b)
{
    return ! (a == b);
}

QStringList & operator >> (QStringList & in, djvStyle::Palette & out) throw (QString)
{
    in >> out.name;
    QString tmp;
    in >> tmp;
    in >> out.foreground;
    in >> tmp;
    in >> out.background;
    in >> tmp;
    in >> out.background2;
    in >> tmp;
    in >> out.button;
    in >> tmp;
    in >> out.select;
    
    return in;
}

QStringList & operator << (QStringList & out, const djvStyle::Palette & in)
{
    out << in.name;
    out << QString("foreground");
    out << in.foreground;
    out << QString("background");
    out << in.background;
    out << QString("background2");
    out << in.background2;
    out << QString("button");
    out << in.button;
    out << QString("select");
    out << in.select;
    
    return out;
}

QStringList & operator >> (QStringList & in, djvStyle::SizeMetric & out)
    throw (QString)
{
    in >> out.name;
    QString tmp;
    in >> tmp;
    in >> out.fontSize;
    in >> tmp;
    in >> out.pickSize;
    in >> tmp;
    in >> out.handleSize;
    in >> tmp;
    in >> out.spacing;
    in >> tmp;
    in >> out.largeSpacing;
    in >> tmp;
    in >> out.margin;
    in >> tmp;
    in >> out.textMargin;
    in >> tmp;
    in >> out.widgetMargin;
    in >> tmp;
    in >> out.largeMargin;
    in >> tmp;
    in >> out.iconSize;
    in >> tmp;
    in >> out.toolIconSize;
    in >> tmp;
    in >> out.buttonSize;
    in >> tmp;
    in >> out.textSize;
    in >> tmp;
    in >> out.swatchSize;
    in >> tmp;
    in >> out.thumbnailSize;
    
    return in;
}

QStringList & operator << (QStringList & out, const djvStyle::SizeMetric & in)
{
    out << in.name;
    out << QString("fontSize");
    out << in.fontSize;
    out << QString("pickSize");
    out << in.pickSize;
    out << QString("handleSize");
    out << in.handleSize;
    out << QString("spacing");
    out << in.spacing;
    out << QString("largeSpacing");
    out << in.largeSpacing;
    out << QString("margin");
    out << in.margin;
    out << QString("textMargin");
    out << in.textMargin;
    out << QString("widgetMargin");
    out << in.widgetMargin;
    out << QString("largeMargin");
    out << in.largeMargin;
    out << QString("iconSize");
    out << in.iconSize;
    out << QString("toolIconSize");
    out << in.toolIconSize;
    out << QString("buttonSize");
    out << in.buttonSize;
    out << QString("textSize");
    out << in.textSize;
    out << QString("swatchSize");
    out << in.swatchSize;
    out << QString("thumbnailSize");
    out << in.thumbnailSize;
    
    return out;
}

QStringList & operator >> (QStringList & in, djvStyle::Fonts & out)
    throw (QString)
{
    QString tmp, tmp2;
    in >> tmp;
    in >> tmp2;
    out.normal.setFamily(tmp2);
    in >> tmp;
    in >> tmp2;
    out.fixed.setFamily(tmp2);

    return in;
}

QStringList & operator << (QStringList & out, const djvStyle::Fonts & in)
{
    out << QString("normal");
    out << in.normal.family();
    out << QString("fixed");
    out << in.fixed.family();
    
    return out;
}
