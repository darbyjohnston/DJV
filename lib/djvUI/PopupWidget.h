// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a popup widget.
        class PopupWidget : public Widget
        {
            DJV_NON_COPYABLE(PopupWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PopupWidget();

        public:
            virtual ~PopupWidget();

            static std::shared_ptr<PopupWidget> create(const std::shared_ptr<Core::Context>&);

            void open();
            void close();

            const std::string & getIcon() const;
            const std::string& getPopupIcon() const;
            void setIcon(const std::string &);
            void setPopupIcon(const std::string&);

            const std::string & getText() const;
            void setText(const std::string &);

            const std::string& getFont() const;
            const std::string& getFontFace() const;
            MetricsRole getFontSizeRole() const;
            void setFont(const std::string&);
            void setFontFace(const std::string&);
            void setFontSizeRole(MetricsRole);

            MetricsRole getInsideMargin() const;
            void setInsideMargin(MetricsRole);

            bool hasCapturePointer() const;
            bool hasCaptureKeyboard() const;
            void setCapturePointer(bool);
            void setCaptureKeyboard(bool);

            void setOpenCallback(const std::function<void(void)>&);
            void setCloseCallback(const std::function<void(void)>&);

            void addChild(const std::shared_ptr<IObject> &) override;
            void removeChild(const std::shared_ptr<IObject> &) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

