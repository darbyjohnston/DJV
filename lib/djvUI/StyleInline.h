// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            inline const Image::Color& Palette::getColor(ColorRole value) const
            {
                return _colors.at(value);
            }

            inline float Palette::getDisabledMult() const
            {
                return _disabledMult;
            }

            inline float Metrics::getMetric(MetricsRole role) const
            {
                return _metrics.at(role);
            }

            inline const Palette& Style::getPalette() const
            {
                return _palette;
            }

            inline float Style::getBrightness() const
            {
                return _brightness;
            }

            inline float Style::getContrast() const
            {
                return _contrast;
            }

            inline const glm::vec2& Style::getDPI() const
            {
                return _dpi;
            }

            inline const Metrics& Style::getMetrics() const
            {
                return _metrics;
            }

            inline float Style::getScale() const
            {
                return _dpi.x / static_cast<float>(Render2D::dpiDefault);
            }

            inline float Style::getMetric(MetricsRole role) const
            {
                return ceilf(_metrics.getMetric(role) * getScale());
            }

            inline const std::string Style::getFont() const
            {
                return _font;
            }

            inline bool Style::isPaletteDirty() const
            {
                return _paletteDirty;
            }

            inline bool Style::isSizeDirty() const
            {
                return _sizeDirty;
            }

            inline bool Style::isFontDirty() const
            {
                return _fontDirty;
            }

        } // namespace Style
    } // namespace UI
} // namespace djv
