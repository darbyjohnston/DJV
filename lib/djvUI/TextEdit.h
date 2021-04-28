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
        namespace Text
        {
            //! Text editor widget.
            class Edit : public Widget
            {
                DJV_NON_COPYABLE(Edit);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Edit();

            public:
                ~Edit() override;

                static std::shared_ptr<Edit> create(const std::shared_ptr<System::Context>&);

                //! \name Text
                ///@{

                const std::string& getText() const;

                void setText(const std::string&);
                void appendText(const std::string&);
                void clearText();

                ///@}

                //! \name Font
                ///@{

                const std::string& getFontFamily() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;

                void setFontFamily(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                ///@}

                //! \name Options
                ///@{

                MetricsRole getTextSizeRole() const;

                void setTextSizeRole(MetricsRole);
                void setBorder(bool);

                ///@}

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Text
    } // namespace UI
} // namespace djv
