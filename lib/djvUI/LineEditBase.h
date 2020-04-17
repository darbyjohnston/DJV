// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/Range.h>

namespace djv
{
    namespace UI
    {
        //! This class provides the base functionality for line edit widgets.
        class LineEditBase : public Widget
        {
            DJV_NON_COPYABLE(LineEditBase);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LineEditBase();

        public:
            virtual ~LineEditBase();

            static std::shared_ptr<LineEditBase> create(const std::shared_ptr<Core::Context>&);

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
            void setTextEditCallback(const std::function<void(const std::string&, TextEditReason)> &);
            void setFocusCallback(const std::function<void(bool)> &);

            bool acceptFocus(TextFocusDirection) override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _clipEvent(Core::Event::Clip &) override;
            void _paintEvent(Core::Event::Paint &) override;
            void _pointerEnterEvent(Core::Event::PointerEnter &) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
            void _pointerMoveEvent(Core::Event::PointerMove &) override;
            void _buttonPressEvent(Core::Event::ButtonPress &) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _textFocusEvent(Core::Event::TextFocus &) override;
            void _textFocusLostEvent(Core::Event::TextFocusLost &) override;
            void _textInputEvent(Core::Event::TextInput&) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            std::string _fromUtf32(const std::basic_string<djv_char_t>&);
            std::basic_string<djv_char_t> _toUtf32(const std::string&);
            
            Core::SizeTRange _getSelection() const;

            void _textUpdate();
            void _cursorUpdate();
            void _viewUpdate();

            void _doTextChangedCallback();
            void _doTextEditCallback(TextEditReason);
            void _doFocusCallback(bool);

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

