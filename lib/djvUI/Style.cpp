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

#include <djvUI/Style.h>

#include <djvUI/Prefs.h>

#include <djvGraphics/ColorUtil.h>

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPalette>
#include <QStyleFactory>
#include <QStyleOption>

namespace djv
{
    namespace UI
    {
        Style::Palette::Palette(
            const QString &  name,
            const Graphics::Color & foreground,
            const Graphics::Color & background,
            const Graphics::Color & background2,
            const Graphics::Color & button,
            const Graphics::Color & select) :
            name(name),
            foreground(foreground),
            background(background),
            background2(background2),
            button(button),
            select(select)
        {}

        Style::SizeMetric::SizeMetric(const QString & name, int fontSize, int iconDPI, int iconSize, int toolIconSize) :
            name(name),
            fontSize(fontSize),
            pickSize(static_cast<int>(fontSize * .8f)),
            handleSize(static_cast<int>(fontSize * 3.f)),
            spacing(static_cast<int>(fontSize * .5f)),
            largeSpacing(fontSize),
            margin(static_cast<int>(fontSize * .5f)),
            textMargin(static_cast<int>(fontSize * .5f)),
            widgetMargin(static_cast<int>(fontSize * .5f)),
            largeMargin(fontSize),
            iconDPI(iconDPI),
            iconSize(iconSize),
            toolIconSize(toolIconSize),
            buttonSize(static_cast<int>(fontSize * 4)),
            textSize(static_cast<int>(fontSize * 10)),
            swatchSize(static_cast<int>(fontSize * 4)),
            thumbnailSize(static_cast<int>(fontSize * 10))
        {}

        Style::Fonts::Fonts() :
            normal(qApp->font()),
            fixed("Monospace")
        {
            fixed.setStyleHint(QFont::Monospace);
        }

        struct Style::Private
        {
            std::vector<Style::Palette> palettes = Style::palettesDefault();
            int palettesIndex = Style::palettesIndexDefault();
            bool colorSwatchTransparency = Style::colorSwatchTransparencyDefault();
            std::vector<Style::SizeMetric> sizeMetrics = Style::sizeMetricsDefault();
            int sizeMetricsIndex = Style::sizeMetricsIndexDefault();
            Style::Fonts fonts;
        };

        Style::Style(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("Style::Style");

            // Load preferences.
            Prefs prefs("djv::UI::Style", Prefs::SYSTEM);
            int paletteCount = 0;
            prefs.get("paletteCount", paletteCount);
            //DJV_DEBUG_PRINT("paletteCount = " << paletteCount);
            for (int i = 0; i < paletteCount; ++i)
            {
                Palette palette;
                prefs.get(QString("palette%1").arg(i), palette);
                //DJV_DEBUG_PRINT("palette = " << palette.name);
                size_t j = 0;
                for (; j < _p->palettes.size(); ++j)
                {
                    if (palette.name == _p->palettes[j].name)
                    {
                        _p->palettes[j] = palette;
                        break;
                    }
                }
                if (j == _p->palettes.size())
                {
                    _p->palettes.push_back(palette);
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
                size_t j = 0;
                for (; j < _p->sizeMetrics.size(); ++j)
                {
                    if (sizeMetric.name == _p->sizeMetrics[j].name)
                    {
                        _p->sizeMetrics[j] = sizeMetric;
                        break;
                    }
                }
                if (j == _p->sizeMetrics.size())
                {
                    _p->sizeMetrics.push_back(sizeMetric);
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

        Style::~Style()
        {
            //DJV_DEBUG("Style::~Style");

            // Save preferences.
            Prefs prefs("djv::UI::Style", Prefs::SYSTEM);
            const int paletteCount = static_cast<int>(_p->palettes.size());
            prefs.set("paletteCount", static_cast<int>(paletteCount));
            for (int i = 0; i < paletteCount; ++i)
            {
                prefs.set(QString("palette%1").arg(i), _p->palettes[i]);
            }
            //DJV_DEBUG_PRINT("palettesIndex = " << _p->palettesIndex);
            prefs.set("palettesIndex", _p->palettesIndex);

            const int sizeMetricsCount = static_cast<int>(_p->sizeMetrics.size());
            prefs.set("sizeMetricsCount", sizeMetricsCount);
            for (int i = 0; i < sizeMetricsCount; ++i)
            {
                prefs.set(QString("sizeMetric%1").arg(i), _p->sizeMetrics[i]);
            }
            prefs.set("sizeMetricsIndex", _p->sizeMetricsIndex);

            prefs.set("fonts", _p->fonts);
        }

        const std::vector<Style::Palette> & Style::palettesDefault()
        {
            static const std::vector<Style::Palette> data =
            {
                Palette(
                    qApp->translate("djv::UI::Style", "Dark"),
                    Graphics::Color(.9f),
                    Graphics::Color(.2f),
                    Graphics::Color(.15f),
                    Graphics::Color(.25f),
                    Graphics::Color(.7f, .6f, .3f)),
                Palette(
                    qApp->translate("djv::UI::Style", "Light"),
                    Graphics::Color(.1f),
                    Graphics::Color(.9f),
                    Graphics::Color(.85f),
                    Graphics::Color(.8f),
                    Graphics::Color(.7f, .6f, .3f)),
                Palette(
                    qApp->translate("djv::UI::Style", "Default"),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Foreground)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Background)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Base)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Button)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Highlight))),
                Palette(
                    qApp->translate("djv::UI::Style", "Vaporwave"),
                    Graphics::Color(.05f, .96f, .93f),
                    Graphics::Color(0.f, .06f, .27f),
                    Graphics::Color(0.f, .02f, .1f),
                    Graphics::Color(.44f, .01f, .62f),
                    Graphics::Color(.05f, .22f, .36f)),
                Palette(
                    qApp->translate("djv::UI::Style", "Custom"),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Foreground)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Background)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Base)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Button)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Highlight)))
            };
            return data;
        }

        const std::vector<Style::Palette> & Style::palettes()
        {
            return _p->palettes;
        }

        void Style::setPalettes(const std::vector<Palette> & palettes)
        {
            //! \bug Error C2893 on Windows?
            //if (palettes == _p->palettes)
            //    return;
            if (palettes.size() == _p->palettes.size())
            {
                size_t i = 0;
                for (; i < palettes.size(); ++i)
                {
                    if (palettes[i] != _p->palettes[i])
                    {
                        break;
                    }
                }
                if (i == palettes.size())
                {
                    return;
                }
            }
            _p->palettes = palettes;
            colorUpdate();
        }

        int Style::palettesIndexDefault()
        {
            return 0;
        }

        int Style::palettesIndex() const
        {
            return _p->palettesIndex;
        }

        void Style::setPalettesIndex(int index)
        {
            if (index == _p->palettesIndex)
                return;
            _p->palettesIndex = index;
            colorUpdate();
        }

        const Style::Palette & Style::palette() const
        {
            return _p->palettes[_p->palettesIndex];
        }

        void Style::setPalette(const Palette & palette)
        {
            if (palette == _p->palettes[_p->palettesIndex])
                return;
            _p->palettes[_p->palettesIndex] = palette;
            colorUpdate();
        }

        QStringList Style::paletteNames() const
        {
            QStringList names;
            for (size_t i = 0; i < _p->palettes.size(); ++i)
            {
                names += _p->palettes[i].name;
            }
            return names;
        }

        bool Style::colorSwatchTransparencyDefault()
        {
            return true;
        }

        bool Style::hasColorSwatchTransparency() const
        {
            return _p->colorSwatchTransparency;
        }

        void Style::setColorSwatchTransparency(bool in)
        {
            if (in == _p->colorSwatchTransparency)
                return;
            _p->colorSwatchTransparency = in;
            Q_EMIT colorSwatchTransparencyChanged(_p->colorSwatchTransparency);
        }

        const std::vector<Style::SizeMetric> & Style::sizeMetricsDefault()
        {
            static const std::vector<SizeMetric> data =
            {
                SizeMetric(qApp->translate("djv::UI::Style", "Default"), 12, 96, 25, 20),
                SizeMetric(qApp->translate("djv::UI::Style", "Large"), 24, 192, 50, 40)
            };
            return data;
        }

        const std::vector<Style::SizeMetric> & Style::sizeMetrics()
        {
            return _p->sizeMetrics;
        }

        void Style::setSizeMetrics(const std::vector<SizeMetric> & sizeMetrics)
        {
            //! \bug Error C2893 on Windows?
            //if (sizeMetrics == _p->sizeMetrics)
            //    return;
            if (sizeMetrics.size() == _p->sizeMetrics.size())
            {
                size_t i = 0;
                for (; i < sizeMetrics.size(); ++i)
                {
                    if (sizeMetrics[i] != _p->sizeMetrics[i])
                    {
                        break;
                    }
                }
                if (i == sizeMetrics.size())
                {
                    return;
                }
            }
            _p->sizeMetrics = sizeMetrics;
            sizeUpdate();
            Q_EMIT sizeMetricsChanged();
        }

        int Style::sizeMetricsIndexDefault()
        {
            return 0;
        }

        int Style::sizeMetricsIndex() const
        {
            return _p->sizeMetricsIndex;
        }

        void Style::setSizeMetricsIndex(int index)
        {
            if (index == _p->sizeMetricsIndex)
                return;
            _p->sizeMetricsIndex = index;
            sizeUpdate();
            Q_EMIT sizeMetricsChanged();
        }

        const Style::SizeMetric & Style::sizeMetric() const
        {
            return _p->sizeMetrics[_p->sizeMetricsIndex];
        }

        void Style::setSizeMetric(const SizeMetric & sizeMetric)
        {
            if (sizeMetric == _p->sizeMetrics[_p->sizeMetricsIndex])
                return;
            _p->sizeMetrics[_p->sizeMetricsIndex] = sizeMetric;
            sizeUpdate();
            Q_EMIT sizeMetricsChanged();
        }

        QStringList Style::sizeMetricNames() const
        {
            QStringList names;
            for (size_t i = 0; i < _p->sizeMetrics.size(); ++i)
            {
                names += _p->sizeMetrics[i].name;
            }
            return names;
        }

        const Style::Fonts & Style::fontsDefault()
        {
            static const Style::Fonts data;
            return data;
        }

        const Style::Fonts & Style::fonts() const
        {
            return _p->fonts;
        }

        void Style::setFonts(const Fonts & fonts)
        {
            if (fonts == _p->fonts)
                return;
            _p->fonts = fonts;
            fontsUpdate();
            Q_EMIT fontsChanged();
        }

        namespace
        {

            QColor toQColor(const Graphics::Color & color)
            {
                Graphics::Color tmp(Graphics::Pixel::RGBA_U8);
                Graphics::ColorUtil::convert(color, tmp);
                return QColor(tmp.u8(0), tmp.u8(1), tmp.u8(2), tmp.u8(3));
            }

        } // namespace

        void Style::colorUpdate()
        {
            //DJV_DEBUG("Style::colorUpdate");
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

        void Style::sizeUpdate()
        {
            //DJV_DEBUG("Style::sizeUpdate");
            QFont font = _p->fonts.normal;
            font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
            qApp->setFont(font);
        }

        void Style::fontsUpdate()
        {
            //DJV_DEBUG("Style::fontsUpdate");
            QFont font = _p->fonts.normal;
            font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
            qApp->setFont(font);
        }

        ProxyStyle::ProxyStyle() :
            QProxyStyle(QStyleFactory::create("fusion"))
        {}
        
        void ProxyStyle::drawControl(
            ControlElement element,
            const QStyleOption * option,
            QPainter * painter,
            const QWidget * widget) const   
        {
            if (CE_ToolBar == element)
            {
            }
            else
            {
                QProxyStyle::drawControl(element, option, painter, widget);
            }
        }
        
        void ProxyStyle::drawPrimitive(
            PrimitiveElement element,
            const QStyleOption * option,
            QPainter * painter,
            const QWidget * widget) const
        {
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }

    } // namespace UI

    bool operator == (const UI::Style::Palette & a, const UI::Style::Palette & b)
    {
        return
            a.foreground == b.foreground  &&
            a.background == b.background  &&
            a.background2 == b.background2 &&
            a.button == b.button &&
            a.select == b.select;
    }

    bool operator != (const UI::Style::Palette & a, const UI::Style::Palette & b)
    {
        return !(a == b);
    }

    bool operator == (const UI::Style::SizeMetric & a, const UI::Style::SizeMetric & b)
    {
        return
            a.fontSize == b.fontSize &&
            a.pickSize == b.pickSize &&
            a.handleSize == b.handleSize &&
            a.spacing == b.spacing &&
            a.largeSpacing == b.largeSpacing &&
            a.margin == b.margin &&
            a.textMargin == b.textMargin &&
            a.widgetMargin == b.widgetMargin &&
            a.largeMargin == b.largeMargin  &&
            a.iconDPI == b.iconDPI &&
            a.iconSize == b.iconSize &&
            a.toolIconSize == b.toolIconSize &&
            a.buttonSize == b.buttonSize &&
            a.textSize == b.textSize &&
            a.swatchSize == b.swatchSize &&
            a.thumbnailSize == b.thumbnailSize;
    }

    bool operator != (const UI::Style::SizeMetric & a, const UI::Style::SizeMetric & b)
    {
        return !(a == b);
    }

    bool operator == (const UI::Style::Fonts & a, const UI::Style::Fonts & b)
    {
        return
            a.normal == b.normal &&
            a.fixed == b.fixed;
    }

    bool operator != (const UI::Style::Fonts & a, const UI::Style::Fonts & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, UI::Style::Palette & out) throw (QString)
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

    QStringList & operator << (QStringList & out, const UI::Style::Palette & in)
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

    QStringList & operator >> (QStringList & in, UI::Style::SizeMetric & out)
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
        in >> out.iconDPI;
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

    QStringList & operator << (QStringList & out, const UI::Style::SizeMetric & in)
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
        out << QString("iconDPI");
        out << in.iconDPI;
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

    QStringList & operator >> (QStringList & in, UI::Style::Fonts & out)
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

    QStringList & operator << (QStringList & out, const UI::Style::Fonts & in)
    {
        out << QString("normal");
        out << in.normal.family();
        out << QString("fixed");
        out << in.fixed.family();
        return out;
    }

} // namespace djv
