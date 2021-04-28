// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/Range.h>

namespace djv
{
    namespace UI
    {
        namespace Text
        {
            //! Base class for line edit widgets.
            //!
            //! \todo Add a class for constraining the minimum size of multiple
            //! LineEditBase widgets, similiar to LabelSizeGroup.
            class LineEditBase : public Widget
            {
                DJV_NON_COPYABLE(LineEditBase);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                LineEditBase();

            public:
                ~LineEditBase() override;

                static std::shared_ptr<LineEditBase> create(const std::shared_ptr<System::Context>&);

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

                bool acceptFocus(TextFocusDirection) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _clipEvent(System::Event::Clip&) override;
                void _paintEvent(System::Event::Paint&) override;
                void _pointerEnterEvent(System::Event::PointerEnter&) override;
                void _pointerLeaveEvent(System::Event::PointerLeave&) override;
                void _pointerMoveEvent(System::Event::PointerMove&) override;
                void _buttonPressEvent(System::Event::ButtonPress&) override;
                void _buttonReleaseEvent(System::Event::ButtonRelease&) override;
                void _keyPressEvent(System::Event::KeyPress&) override;
                void _textFocusEvent(System::Event::TextFocus&) override;
                void _textFocusLostEvent(System::Event::TextFocusLost&) override;
                void _textInputEvent(System::Event::TextInput&) override;

                void _initEvent(System::Event::Init&) override;
                void _updateEvent(System::Event::Update&) override;

            private:
                std::string _fromUtf32(const std::basic_string<djv_char_t>&);
                std::basic_string<djv_char_t> _toUtf32(const std::string&);

                Math::SizeTRange _getSelection() const;

                void _textUpdate();
                void _cursorUpdate();
                void _viewUpdate();

                void _doTextChangedCallback();
                void _doTextEditCallback(TextEditReason);
                void _doFocusCallback(bool);

                DJV_PRIVATE();
            };

        } // namespace Text
    } // namespace UI
} // namespace djv

