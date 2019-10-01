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

#pragma once

#include <djvUI/Enum.h>

#include <djvCore/BBox.h>

#include <memory>

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            class Style;
            
        } // namespace Style
        
        namespace Layout
        {
            //! This class provides margins.
            class Margin
            {
            public:
                Margin();
                Margin(MetricsRole);
                //! \todo Change the ordering to match the Side enum? (left, top, right, bottom)
                Margin(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom);

                void set(MetricsRole);
                void set(MetricsRole left, MetricsRole right, MetricsRole top, MetricsRole bottom);

                float get(Side, const std::shared_ptr<Style::Style> &) const;
                glm::vec2 getSize(const std::shared_ptr<Style::Style> &) const;
                float getWidth(const std::shared_ptr<Style::Style> &) const;
                float getHeight(const std::shared_ptr<Style::Style> &) const;

                Core::BBox2f bbox(const Core::BBox2f &, const std::shared_ptr<Style::Style> &) const;

                MetricsRole operator [] (Side) const;
                MetricsRole & operator [] (Side);

                bool operator == (const Margin &) const;

            private:
                MetricsRole _value[4] =
                {
                    MetricsRole::None,
                    MetricsRole::None,
                    MetricsRole::None,
                    MetricsRole::None
                };
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

#include <djvUI/MarginInline.h>
