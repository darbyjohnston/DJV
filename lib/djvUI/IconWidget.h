// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
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
        //! Icon widget.
        class IconWidget : public Widget
        {
            DJV_NON_COPYABLE(IconWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            IconWidget();

        public:
            ~IconWidget() override;

            static std::shared_ptr<IconWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Icon
            ///@{

            const std::string& getIcon() const;

            void setIcon(const std::string&);

            ///@}

            //! \name Options
            ///@{

            ColorRole getIconColorRole() const;
            const Image::Color& getIconColor() const;
            MetricsRole getIconSizeRole() const;

            void setIconColorRole(ColorRole);
            void setIconColor(const Image::Color&);
            void setIconSizeRole(MetricsRole);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init&) override;
            void _updateEvent(System::Event::Update&) override;

        private:
            void _iconUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

