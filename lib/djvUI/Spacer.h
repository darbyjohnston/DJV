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

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a spacer for use in layouts.
        class Spacer : public Widget
        {
            DJV_NON_COPYABLE(Spacer);

        protected:
            void _init(Core::Context *);
            Spacer();

        public:
            virtual ~Spacer();

            static std::shared_ptr<Spacer> create(Orientation, Core::Context *);

            Orientation getOrientation() const;
            void setOrientation(Orientation);

            MetricsRole getSpacerSize() const;
            void setSpacerSize(MetricsRole);

            void preLayoutEvent(Core::PreLayoutEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides a horizontal spacer for use in layouts.
        class HorizontalSpacer : public Spacer
        {
            DJV_NON_COPYABLE(HorizontalSpacer);

        protected:
            HorizontalSpacer();

        public:
            static std::shared_ptr<HorizontalSpacer> create(Core::Context *);
        };

        //! This class provides a vertical spacer for use in layouts.
        class VerticalSpacer : public Spacer
        {
            DJV_NON_COPYABLE(VerticalSpacer);

        protected:
            VerticalSpacer();

        public:
            static std::shared_ptr<VerticalSpacer> create(Core::Context *);
        };

    } // namespace UI
} // namespace djv
