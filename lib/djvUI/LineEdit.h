// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Text
        {
            //! Line edit widget.
            class LineEdit : public Widget
            {
                DJV_NON_COPYABLE(LineEdit);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                LineEdit();

            public:
                ~LineEdit() override;

                static std::shared_ptr<LineEdit> create(const std::shared_ptr<System::Context>&);

                //! \name Text
                ///@{

                const std::string& getText() const;

                void setText(const std::string&);

                ///@}

                //! \name Font
                ///@{

                const std::string& getFont() const;
                const std::string& getFontFace() const;
                MetricsRole getFontSizeRole() const;

                void setFont(const std::string&);
                void setFontFace(const std::string&);
                void setFontSizeRole(MetricsRole);

                ///@}

                //! \name Options
                ///@{

                ColorRole getTextColorRole() const;

                void setTextColorRole(ColorRole);

                ///@}

                //! \name Size
                ///@{

                MetricsRole getTextSizeRole() const;
                const std::string& getSizeString() const;

                void setTextSizeRole(MetricsRole);
                void setSizeString(const std::string&);

                ///@}

                //! \name Callbacks
                ///@{

                void setTextChangedCallback(const std::function<void(const std::string&)>&);
                void setTextEditCallback(const std::function<void(const std::string&, TextEditReason)>&);
                void setFocusCallback(const std::function<void(bool)>&);

                ///@}

                void setBackgroundColorRole(ColorRole) override;

                std::shared_ptr<Widget> getFocusWidget() override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Text
    } // namespace UI
} // namespace djv

