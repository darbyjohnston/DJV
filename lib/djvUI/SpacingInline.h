//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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
        inline Spacing::Spacing()
        {}

        inline Spacing::Spacing(MetricsRole value)
        {
            set(value);
        }

        inline Spacing::Spacing(MetricsRole horizontal, MetricsRole vertical)
        {
            set(horizontal, vertical);
        }

        inline void Spacing::set(MetricsRole value)
        {
            _value[static_cast<size_t>(Orientation::Horizontal)] = value;
            _value[static_cast<size_t>(Orientation::Vertical)]   = value;
        }

        inline void Spacing::set(MetricsRole horizontal, MetricsRole vertical)
        {
            _value[static_cast<size_t>(Orientation::Horizontal)] = horizontal;
            _value[static_cast<size_t>(Orientation::Vertical)]   = vertical;
        }

        inline glm::vec2 Spacing::get(const std::shared_ptr<Style>& style) const
        {
            return glm::vec2(get(Orientation::Horizontal, style), get(Orientation::Vertical, style));
        }
        
        inline float Spacing::get(Orientation value, const std::shared_ptr<Style>& style) const
        {
            return style->getMetric(_value[static_cast<size_t>(value)]);
        }
        
        inline MetricsRole Spacing::operator [] (Orientation value) const
        {
            return _value[static_cast<size_t>(value)];
        }

        inline MetricsRole& Spacing::operator [] (Orientation value)
        {
            return _value[static_cast<size_t>(value)];
        }

        inline bool Spacing::operator == (const Spacing& other) const
        {
            return
                _value[static_cast<size_t>(Orientation::Horizontal)] == other._value[static_cast<size_t>(Orientation::Horizontal)] &&
                _value[static_cast<size_t>(Orientation::Vertical)]   == other._value[static_cast<size_t>(Orientation::Vertical)];
        }

    } // namespace UI
} // namespace djv

