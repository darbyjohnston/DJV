// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace Image
    {
        class Color;
    
    } // namespace Image

    namespace UI
    {
        namespace Button
        {
            //! This class provides a button to popup a widget.
            class Popup : public Widget
            {
                DJV_NON_COPYABLE(Popup);

            protected:
                void _init(MenuButtonStyle, const std::shared_ptr<System::Context>&);
                Popup();

            public:
                ~Popup() override;

                static std::shared_ptr<Popup> create(MenuButtonStyle, const std::shared_ptr<System::Context>&);

                //! \name Open and Close
                ///@{

                void open();
                void close();

                void setPopupDefault(UI::Popup);

                ///@}

                //! \name Icon
                ///@{

                std::string getIcon() const;
                std::string getPopupIcon() const;

                void setIcon(const std::string&);
                void setPopupIcon(const std::string&);

                ColorRole getIconColorRole() const;
                const Image::Color& getIconColor() const;

                void setIconColorRole(ColorRole);
                void setIconColor(const Image::Color&);

                ///@}

                //! \name Text
                ///@{

                const std::string& getText() const;

                void setText(const std::string&);

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

                size_t getTextElide() const;
                MetricsRole getInsideMargin() const;
                bool hasCapturePointer() const;
                bool hasCaptureKeyboard() const;

                void setTextElide(size_t);
                void setInsideMargin(MetricsRole);
                void setCapturePointer(bool);
                void setCaptureKeyboard(bool);

                ///@}

                //! \name Callbacks
                ///@{

                void setOpenCallback(const std::function<std::shared_ptr<Widget>(void)>&);
                void setCloseCallback(const std::function<void(const std::shared_ptr<Widget>&)>&);

                ///@}

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Popup PopupButton;

    } // namespace UI
} // namespace djv

