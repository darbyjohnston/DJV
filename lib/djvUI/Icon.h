// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a widget that displays an icon.
        class Icon : public Widget
        {
            DJV_NON_COPYABLE(Icon);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Icon();

        public:
            ~Icon() override;

            static std::shared_ptr<Icon> create(const std::shared_ptr<System::Context>&);

            const std::string& getIcon() const;
            void setIcon(const std::string&);

            ColorRole getIconColorRole() const;
            void setIconColorRole(ColorRole);

            MetricsRole getIconSizeRole() const;
            void setIconSizeRole(MetricsRole);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init&) override;
            void _updateEvent(System::Event::Update&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

