// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! Magnify system.
        class MagnifySystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(MagnifySystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MagnifySystem();

        public:
            ~MagnifySystem() override;

            static std::shared_ptr<MagnifySystem> create(const std::shared_ptr<System::Context>&);

            int getSortKey() const override;
            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetActions() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetToolBarActions() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            void deleteToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

