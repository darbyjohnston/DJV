// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IWidget.h>

namespace djv
{
    namespace app
    {
        //! Shuttle widget.
        class ShuttleWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(ShuttleWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::string& iconPrefix,
                const std::shared_ptr<IWidget>& parent);

            ShuttleWidget();

        public:
            ~ShuttleWidget();

            static std::shared_ptr<ShuttleWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::string& iconPrefix,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setActiveCallback(const std::function<void(bool)>&);
            void setCallback(const std::function<void(int)>&);

            void sizeHintEvent(const dtk::SizeHintEvent&) override;
            void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
            void mouseEnterEvent(dtk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(dtk::MouseMoveEvent&) override;
            void mousePressEvent(dtk::MouseClickEvent&) override;
            void mouseReleaseEvent(dtk::MouseClickEvent&) override;

        private:
            DTK_PRIVATE();
        };
    }
}
