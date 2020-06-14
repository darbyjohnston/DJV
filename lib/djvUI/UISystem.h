// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace UI
    {
        namespace Style
        {
            class Style;

        } // namespace Style

        //! This class provides a UI system.
        class UISystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(UISystem);

        protected:
            void _init(bool resetSettings, const std::shared_ptr<Core::Context>&);
            UISystem();

        public:
            ~UISystem() override;

            static std::shared_ptr<UISystem> create(bool resetSettings, const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<Style::Style>& getStyle() const;

        private:

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
