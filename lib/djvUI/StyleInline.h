//------------------------------------------------------------------------------
// Copyright (c) 2019-2020 Darby Johnston
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

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            inline const AV::Image::Color& Palette::getColor(ColorRole value) const
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

            inline const AV::Image::Color& Style::getColor(ColorRole role) const
            {
                return _palette.getColor(role);
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
                return _dpi.x / static_cast<float>(AV::dpiDefault);
            }

            inline float Style::getMetric(MetricsRole role) const
            {
                return ceilf(_metrics.getMetric(role) * getScale());
            }

            inline const std::string Style::getFont() const
            {
                return _font;
            }

            inline bool Style::isDirty() const
            {
                return _dirty;
            }

        } // namespace Style
    } // namespace UI
} // namespace djv
