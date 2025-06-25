// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/IMenuPopup.h>

namespace djv
{
    namespace app
    {
        //! Speed popup.
        class SpeedPopup : public feather_tk::IMenuPopup
        {
            FEATHER_TK_NON_COPYABLE(SpeedPopup);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                double defaultSpeed,
                const std::shared_ptr<IWidget>& parent);

            SpeedPopup();

        public:
            virtual ~SpeedPopup();

            static std::shared_ptr<SpeedPopup> create(
                const std::shared_ptr<feather_tk::Context>&,
                double defaultSpeed,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setCallback(const std::function<void(double)>&);

        private:
            void _widgetUpdate();

            FEATHER_TK_PRIVATE();
        };
    }
}
