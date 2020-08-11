// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Margin.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a text editor widget.
        class TextEdit : public Widget
        {
            DJV_NON_COPYABLE(TextEdit);
            
        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TextEdit();

        public:
            ~TextEdit() override;

            static std::shared_ptr<TextEdit> create(const std::shared_ptr<Core::Context>&);

            const std::string& getText() const;
            void setText(const std::string&);
            void appendText(const std::string&);
            void clearText();

            MetricsRole getTextSizeRole() const;
            void setTextSizeRole(MetricsRole);

            const std::string& getFontFamily() const;
            const std::string& getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFontFamily(const std::string&);
            void setFontFace(const std::string&);
            void setFontSizeRole(MetricsRole);

            void setBorder(bool);
            
            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
