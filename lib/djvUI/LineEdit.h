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

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a widget for editing a line of text.
        class LineEdit : public Widget
        {
            DJV_NON_COPYABLE(LineEdit);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LineEdit();

        public:
            virtual ~LineEdit();

            static std::shared_ptr<LineEdit> create(const std::shared_ptr<Core::Context>&);

            const std::string & getText() const;
            void setText(const std::string &);

            ColorRole getTextColorRole() const;
            MetricsRole getTextSizeRole() const;
            void setTextColorRole(ColorRole);
            void setTextSizeRole(MetricsRole);

            const std::string & getFont() const;
            const std::string & getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFont(const std::string &);
            void setFontFace(const std::string &);
            void setFontSizeRole(MetricsRole);

            const std::string & getSizeString() const;
            void setSizeString(const std::string &);

            void setTextChangedCallback(const std::function<void(const std::string &)> &);
            void setTextFinishedCallback(const std::function<void(const std::string&, TextFinished)> &);
            void setFocusCallback(const std::function<void(bool)> &);

            void setBackgroundRole(ColorRole) override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

