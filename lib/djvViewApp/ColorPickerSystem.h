// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IToolSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the color picker system.
        class ColorPickerSystem : public IToolSystem
        {
            DJV_NON_COPYABLE(ColorPickerSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ColorPickerSystem();

        public:
            ~ColorPickerSystem() override;

            static std::shared_ptr<ColorPickerSystem> create(const std::shared_ptr<System::Context>&);

            ToolActionData getToolAction() const override;
            void setCurrentTool(bool, int) override;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;

        protected:
            void _closeWidget(const std::string&) override;
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

