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
            //! Text block widget.
            class Block : public Widget
            {
                DJV_NON_COPYABLE(Block);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Block();

            public:
                ~Block() override;

                static std::shared_ptr<Block> create(const std::shared_ptr<System::Context>&);

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

                TextHAlign getTextHAlign() const;
                ColorRole getTextColorRole() const;
                MetricsRole getTextSizeRole() const;

                void setTextHAlign(TextHAlign);
                void setTextColorRole(ColorRole);
                void setTextSizeRole(MetricsRole);
                void setWordWrap(bool);

                ///@}

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _clipEvent(System::Event::Clip&) override;
                void _paintEvent(System::Event::Paint&) override;

                void _initEvent(System::Event::Init&) override;
                void _updateEvent(System::Event::Update&) override;

            private:
                void _textUpdate();

                DJV_PRIVATE();
            };

        } // namespace Text
    } // namespace UI
} // namespace djv
