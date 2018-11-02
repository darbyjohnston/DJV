//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/StylePrefs.h>

#include <djvUI/Prefs.h>

#include <djvGraphics/ColorUtil.h>

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QWidget>
#include <QWindow>

namespace djv
{
    namespace UI
    {
        StylePrefs::Palette::Palette(
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

        StylePrefs::SizeMetric::SizeMetric(const QString & name, int fontSize) :
            name(name),
            fontSize(fontSize)
        {}

        StylePrefs::Fonts::Fonts() :
            normal("Noto Sans"),
            fixed("Noto Mono")
        {
            fixed.setStyleHint(QFont::Monospace);
        }

        struct StylePrefs::Private
        {
            std::vector<StylePrefs::Palette> palettes = StylePrefs::palettesDefault();
            int palettesIndex = StylePrefs::palettesIndexDefault();
            bool colorSwatchTransparency = StylePrefs::colorSwatchTransparencyDefault();
            std::vector<StylePrefs::SizeMetric> sizeMetrics = StylePrefs::sizeMetricsDefault();
            int sizeMetricsIndex = StylePrefs::sizeMetricsIndexDefault();
            StylePrefs::Fonts fonts;
        };

        StylePrefs::StylePrefs(QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            //DJV_DEBUG("StylePrefs::StylePrefs");

            Prefs prefs("djv::UI::StylePrefs", Prefs::SYSTEM);
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

        StylePrefs::~StylePrefs()
        {
            //DJV_DEBUG("StylePrefs::~StylePrefs");

            Prefs prefs("djv::UI::StylePrefs", Prefs::SYSTEM);
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

        const std::vector<StylePrefs::Palette> & StylePrefs::palettesDefault()
        {
            static const std::vector<StylePrefs::Palette> data =
            {
                Palette(
                    qApp->translate("djv::UI::StylePrefs", "Dark"),
                    Graphics::Color(.9f),
                    Graphics::Color(.2f),
                    Graphics::Color(.15f),
                    Graphics::Color(.25f),
                    Graphics::Color(.7f, .6f, .3f)),
                Palette(
                    qApp->translate("djv::UI::StylePrefs", "Light"),
                    Graphics::Color(.1f),
                    Graphics::Color(.9f),
                    Graphics::Color(.85f),
                    Graphics::Color(.8f),
                    Graphics::Color(.7f, .6f, .3f)),
                Palette(
                    qApp->translate("djv::UI::StylePrefs", "Default"),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Foreground)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Background)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Base)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Button)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Highlight))),
                Palette(
                    qApp->translate("djv::UI::StylePrefs", "Vaporwave"),
                    Graphics::Color(.05f, .96f, .93f),
                    Graphics::Color(0.f, .06f, .27f),
                    Graphics::Color(0.f, .02f, .1f),
                    Graphics::Color(.44f, .01f, .62f),
                    Graphics::Color(.05f, .22f, .36f)),
                Palette(
                    qApp->translate("djv::UI::StylePrefs", "Custom"),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Foreground)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Background)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Base)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Button)),
                    Graphics::ColorUtil::fromQt(qApp->palette().color(QPalette::Highlight)))
            };
            return data;
        }

        const std::vector<StylePrefs::Palette> & StylePrefs::palettes()
        {
            return _p->palettes;
        }

        void StylePrefs::setPalettes(const std::vector<Palette> & palettes)
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

        int StylePrefs::palettesIndexDefault()
        {
            return 0;
        }

        int StylePrefs::palettesIndex() const
        {
            return _p->palettesIndex;
        }

        void StylePrefs::setPalettesIndex(int index)
        {
            if (index == _p->palettesIndex)
                return;
            _p->palettesIndex = index;
            colorUpdate();
        }

        const StylePrefs::Palette & StylePrefs::palette() const
        {
            return _p->palettes[_p->palettesIndex];
        }

        void StylePrefs::setPalette(const Palette & palette)
        {
            if (palette == _p->palettes[_p->palettesIndex])
                return;
            _p->palettes[_p->palettesIndex] = palette;
            colorUpdate();
        }

        QStringList StylePrefs::paletteNames() const
        {
            QStringList names;
            for (size_t i = 0; i < _p->palettes.size(); ++i)
            {
                names += _p->palettes[i].name;
            }
            return names;
        }

        bool StylePrefs::colorSwatchTransparencyDefault()
        {
            return true;
        }

        bool StylePrefs::hasColorSwatchTransparency() const
        {
            return _p->colorSwatchTransparency;
        }

        void StylePrefs::setColorSwatchTransparency(bool in)
        {
            if (in == _p->colorSwatchTransparency)
                return;
            _p->colorSwatchTransparency = in;
            //! \todo Can we use QWidget::changeEvent() for this instead?
            Q_FOREACH(QWidget * widget, qApp->allWidgets())
            {
                qApp->postEvent(widget, new QEvent(QEvent::StyleChange));
            }
        }

        const std::vector<StylePrefs::SizeMetric> & StylePrefs::sizeMetricsDefault()
        {
            static const std::vector<SizeMetric> data =
            {
                SizeMetric(qApp->translate("djv::UI::StylePrefs", "Default"), 12),
                SizeMetric(qApp->translate("djv::UI::StylePrefs", "Large"), 18)
            };
            return data;
        }

        const std::vector<StylePrefs::SizeMetric> & StylePrefs::sizeMetrics()
        {
            return _p->sizeMetrics;
        }

        void StylePrefs::setSizeMetrics(const std::vector<SizeMetric> & sizeMetrics)
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
        }

        int StylePrefs::sizeMetricsIndexDefault()
        {
            return 0;
        }

        int StylePrefs::sizeMetricsIndex() const
        {
            return _p->sizeMetricsIndex;
        }

        void StylePrefs::setSizeMetricsIndex(int index)
        {
            if (index == _p->sizeMetricsIndex)
                return;
            _p->sizeMetricsIndex = index;
            sizeUpdate();
        }

        const StylePrefs::SizeMetric & StylePrefs::sizeMetric() const
        {
            return _p->sizeMetrics[_p->sizeMetricsIndex];
        }

        void StylePrefs::setSizeMetric(const SizeMetric & sizeMetric)
        {
            if (sizeMetric == _p->sizeMetrics[_p->sizeMetricsIndex])
                return;
            _p->sizeMetrics[_p->sizeMetricsIndex] = sizeMetric;
            sizeUpdate();
        }

        QStringList StylePrefs::sizeMetricNames() const
        {
            QStringList names;
            for (size_t i = 0; i < _p->sizeMetrics.size(); ++i)
            {
                names += _p->sizeMetrics[i].name;
            }
            return names;
        }

        const StylePrefs::Fonts & StylePrefs::fontsDefault()
        {
            static const StylePrefs::Fonts data;
            return data;
        }

        const StylePrefs::Fonts & StylePrefs::fonts() const
        {
            return _p->fonts;
        }

        void StylePrefs::setFonts(const Fonts & fonts)
        {
            if (fonts == _p->fonts)
                return;
            _p->fonts = fonts;
            fontsUpdate();
        }

        void StylePrefs::colorUpdate()
        {
            //DJV_DEBUG("StylePrefs::colorUpdate");
            //DJV_DEBUG_PRINT("foreground = " <<
            //    _p->palettes[_p->palettesIndex].foreground);
            //DJV_DEBUG_PRINT("background = " <<
            //    _p->palettes[_p->palettesIndex].background);
            //DJV_DEBUG_PRINT("background2 = " <<
            //    _p->palettes[_p->palettesIndex].background2);
            QPalette palette;
            palette.setColor(
                QPalette::Window,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].background));
            palette.setColor(
                QPalette::WindowText,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].foreground));
            palette.setColor(
                QPalette::Base,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].background2));
            palette.setColor(
                QPalette::AlternateBase,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].background2).darker(120));
            palette.setColor(
                QPalette::Text,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].foreground));
            palette.setColor(
                QPalette::Button,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].button));
            palette.setColor(
                QPalette::ButtonText,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].foreground));
            palette.setColor(
                QPalette::Highlight,
                Graphics::ColorUtil::toQt(_p->palettes[_p->palettesIndex].select));
            qApp->setPalette(palette);            
            Q_EMIT prefChanged();
            //! \todo Can we use QWidget::changeEvent() for this instead?
            Q_FOREACH(QWidget * widget, qApp->allWidgets())
            {
                qApp->postEvent(widget, new QEvent(QEvent::StyleChange));
            }
        }

        void StylePrefs::sizeUpdate()
        {
            //DJV_DEBUG("StylePrefs::sizeUpdate");
            QFont font = _p->fonts.normal;
            font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
            qApp->setFont(font);
            Q_EMIT prefChanged();
            //! \todo Can we use QWidget::changeEvent() for this instead?
            Q_FOREACH(QWidget * widget, qApp->allWidgets())
            {
                qApp->postEvent(widget, new QEvent(QEvent::StyleChange));
            }
        }

        void StylePrefs::fontsUpdate()
        {
            //DJV_DEBUG("StylePrefs::fontsUpdate");
            QFont font = _p->fonts.normal;
            //DJV_DEBUG_PRINT("font = " << font.family());
            font.setPixelSize(_p->sizeMetrics[_p->sizeMetricsIndex].fontSize);
            qApp->setFont(font);
            Q_EMIT prefChanged();
            //! \todo Can we use QWidget::changeEvent() for this instead?
            Q_FOREACH(QWidget * widget, qApp->allWidgets())
            {
                qApp->postEvent(widget, new QEvent(QEvent::StyleChange));
            }
        }

    } // namespace UI

    bool operator == (const UI::StylePrefs::Palette & a, const UI::StylePrefs::Palette & b)
    {
        return
            a.foreground == b.foreground  &&
            a.background == b.background  &&
            a.background2 == b.background2 &&
            a.button == b.button &&
            a.select == b.select;
    }

    bool operator != (const UI::StylePrefs::Palette & a, const UI::StylePrefs::Palette & b)
    {
        return !(a == b);
    }

    bool operator == (const UI::StylePrefs::SizeMetric & a, const UI::StylePrefs::SizeMetric & b)
    {
        return a.fontSize == b.fontSize;
    }

    bool operator != (const UI::StylePrefs::SizeMetric & a, const UI::StylePrefs::SizeMetric & b)
    {
        return !(a == b);
    }

    bool operator == (const UI::StylePrefs::Fonts & a, const UI::StylePrefs::Fonts & b)
    {
        return
            a.normal == b.normal &&
            a.fixed == b.fixed;
    }

    bool operator != (const UI::StylePrefs::Fonts & a, const UI::StylePrefs::Fonts & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, UI::StylePrefs::Palette & out)
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

    QStringList & operator << (QStringList & out, const UI::StylePrefs::Palette & in)
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

    QStringList & operator >> (QStringList & in, UI::StylePrefs::SizeMetric & out)
    {
        in >> out.name;
        QString tmp;
        in >> tmp;
        in >> out.fontSize;
        return in;
    }

    QStringList & operator << (QStringList & out, const UI::StylePrefs::SizeMetric & in)
    {
        out << in.name;
        out << QString("fontSize");
        out << in.fontSize;
        return out;
    }

    QStringList & operator >> (QStringList & in, UI::StylePrefs::Fonts & out)
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

    QStringList & operator << (QStringList & out, const UI::StylePrefs::Fonts & in)
    {
        out << QString("normal");
        out << in.normal.family();
        out << QString("fixed");
        out << in.fixed.family();
        return out;
    }

} // namespace djv
