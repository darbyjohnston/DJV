// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/IWidget.h>

namespace djv
{
    namespace app
    {
        //! Shuttle widget.
        class ShuttleWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(ShuttleWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::string& iconPrefix,
                const std::shared_ptr<IWidget>& parent);

            ShuttleWidget();

        public:
            ~ShuttleWidget();

            static std::shared_ptr<ShuttleWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::string& iconPrefix,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setActiveCallback(const std::function<void(bool)>&);
            void setCallback(const std::function<void(int)>&);

            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;
            void drawEvent(const feather_tk::Box2I&, const feather_tk::DrawEvent&) override;
            void mouseEnterEvent(feather_tk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(feather_tk::MouseMoveEvent&) override;
            void mousePressEvent(feather_tk::MouseClickEvent&) override;
            void mouseReleaseEvent(feather_tk::MouseClickEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
