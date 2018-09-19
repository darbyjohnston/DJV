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

#pragma once

#include <djvUI/Core.h>

#include <djvGraphics/Color.h>

#include <djvCore/Util.h>

#include <QProxyStyle>
#include <QObject>
#include <QFont>

#include <memory>

namespace djv
{
    namespace UI
    {
        //! \class djvStyle
        //!
        //! This class provides the user interface style.
        //!
        //! \todo Can we use events instead of signals for notifying other objects
        //! when the style changes?
        class Style : public QObject
        {
            Q_OBJECT

        public:
            explicit Style(QObject * parent = nullptr);

            ~Style();

            //! This struct provides a color palette.
            struct Palette
            {
                explicit Palette(
                    const QString &  name = QString(),
                    const Graphics::Color & foreground = Graphics::Color(),
                    const Graphics::Color & background = Graphics::Color(),
                    const Graphics::Color & background2 = Graphics::Color(),
                    const Graphics::Color & button = Graphics::Color(),
                    const Graphics::Color & select = Graphics::Color());

                QString  name;
                Graphics::Color foreground;
                Graphics::Color background;
                Graphics::Color background2;
                Graphics::Color button;
                Graphics::Color select;
            };

            //! Get the default color palettes.
            static const std::vector<Palette> & palettesDefault();

            //! Get the list of color palettes.
            const std::vector<Palette> & palettes();

            //! Set the list of color palettes.
            void setPalettes(const std::vector<Palette> &);

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
            struct SizeMetric
            {
                explicit SizeMetric(
                    const QString & name = QString(),
                    int fontSize = 0,
                    int iconDPI = 0,
                    int iconSize = 0,
                    int toolIconSize = 0);

                QString name;
                int     fontSize = 0;
                int     pickSize = 0;
                int     handleSize = 0;
                int     spacing = 0;
                int     largeSpacing = 0;
                int     margin = 0;
                int     textMargin = 0;
                int     widgetMargin = 0;
                int     largeMargin = 0;
                int     iconDPI = 0;
                int     iconSize = 0;
                int     toolIconSize = 0;
                int     buttonSize = 0;
                int     textSize = 0;
                int     swatchSize = 0;
                int     thumbnailSize = 0;
            };

            //! Get the default size metrics.
            static const std::vector<SizeMetric> & sizeMetricsDefault();

            //! Get the list of size metrics.
            const std::vector<SizeMetric> & sizeMetrics();

            //! Set the list of size metrics.
            void setSizeMetrics(const std::vector<SizeMetric> &);

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
            struct Fonts
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

            DJV_PRIVATE_COPY(Style);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        class ProxyStyle : public QProxyStyle
        {        
            Q_OBJECT

        public:
            void drawControl(ControlElement, const QStyleOption *, QPainter *, const QWidget *) const override;
            void drawPrimitive(PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const override;
        };

    } // namespace UI

    DJV_COMPARISON_OPERATOR(UI::Style::Palette);
    DJV_COMPARISON_OPERATOR(UI::Style::SizeMetric);
    DJV_COMPARISON_OPERATOR(UI::Style::Fonts);
    DJV_STRING_OPERATOR(UI::Style::Palette);
    DJV_STRING_OPERATOR(UI::Style::SizeMetric);
    DJV_STRING_OPERATOR(UI::Style::Fonts);

} // namespace djv
