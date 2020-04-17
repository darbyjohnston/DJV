// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/RowLayout.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a tool bar widget.
        class ToolBar : public Widget
        {
            DJV_NON_COPYABLE(ToolBar);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ToolBar();

        public:
            virtual ~ToolBar();

            static std::shared_ptr<ToolBar> create(const std::shared_ptr<Core::Context>&);

            void setStretch(const std::shared_ptr<Widget> &, RowStretch);

            void addSeparator();
            void addSpacer();
            void addExpander();

            float getHeightForWidth(float) const override;

            void addAction(const std::shared_ptr<Action> &) override;
            void removeAction(const std::shared_ptr<Action> &) override;
            void clearActions() override;

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

