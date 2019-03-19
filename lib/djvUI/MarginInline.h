//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
        namespace Layout
        {
            inline Margin::Margin()
            {}

            inline Margin::Margin(MetricsRole value)
            {
                set(value);
            }

            inline Margin::Margin(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                set(left, right, top, bottom);
            }

            inline void Margin::set(MetricsRole value)
            {
                _value[static_cast<size_t>(Side::Left)] = value;
                _value[static_cast<size_t>(Side::Top)] = value;
                _value[static_cast<size_t>(Side::Right)] = value;
                _value[static_cast<size_t>(Side::Bottom)] = value;
            }

            inline void Margin::set(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom)
            {
                _value[static_cast<size_t>(Side::Left)] = left;
                _value[static_cast<size_t>(Side::Top)] = top;
                _value[static_cast<size_t>(Side::Right)] = right;
                _value[static_cast<size_t>(Side::Bottom)] = bottom;
            }

            inline float Margin::get(Side side, const std::shared_ptr<Style::Style> & style) const
            {
                return style->getMetric(_value[static_cast<size_t>(side)]);
            }

            inline glm::vec2 Margin::getSize(const std::shared_ptr<Style::Style> & style) const
            {
                return glm::vec2(getWidth(style), getHeight(style));
            }

            inline float Margin::getWidth(const std::shared_ptr<Style::Style> & style) const
            {
                return get(Side::Left, style) + get(Side::Right, style);
            }

            inline float Margin::getHeight(const std::shared_ptr<Style::Style> & style) const
            {
                return get(Side::Top, style) + get(Side::Bottom, style);
            }

            inline Core::BBox2f Margin::bbox(const Core::BBox2f & value, const std::shared_ptr<Style::Style> & style) const
            {
                return value.margin(
                    -get(Side::Left, style),
                    -get(Side::Top, style),
                    -get(Side::Right, style),
                    -get(Side::Bottom, style));
            }

            inline MetricsRole Margin::operator [] (Side side) const
            {
                return _value[static_cast<size_t>(side)];
            }

            inline MetricsRole & Margin::operator [] (Side side)
            {
                return _value[static_cast<size_t>(side)];
            }

            inline bool Margin::operator == (const Margin & other) const
            {
                return
                    _value[static_cast<size_t>(Side::Left)] == other._value[static_cast<size_t>(Side::Left)] &&
                    _value[static_cast<size_t>(Side::Top)] == other._value[static_cast<size_t>(Side::Top)] &&
                    _value[static_cast<size_t>(Side::Right)] == other._value[static_cast<size_t>(Side::Right)] &&
                    _value[static_cast<size_t>(Side::Bottom)] == other._value[static_cast<size_t>(Side::Bottom)];
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
