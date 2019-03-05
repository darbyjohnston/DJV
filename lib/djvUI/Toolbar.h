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

#include <djvUI/RowLayout.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a toolbar widget.
        class Toolbar : public Widget
        {
            DJV_NON_COPYABLE(Toolbar);

        protected:
            void _init(Core::Context *);
            Toolbar();

        public:
            virtual ~Toolbar();

            static std::shared_ptr<Toolbar> create(Core::Context *);

            void setStretch(const std::shared_ptr<Widget>&, RowStretch);

            void addSeparator();
            void addSpacer();
            void addExpander();

            float getHeightForWidth(float) const override;

            void addAction(const std::shared_ptr<Action>&) override;
            void removeAction(const std::shared_ptr<Action>&) override;
            void clearActions() override;

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

