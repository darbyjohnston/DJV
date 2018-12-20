// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/IButton.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a toggle button.
        class ToggleButton : public IButton
        {
            DJV_NON_COPYABLE(ToggleButton);

        protected:
            void _init(Core::Context *);
            ToggleButton();

        public:
            virtual ~ToggleButton();

            static std::shared_ptr<ToggleButton> create(Core::Context *);

            void preLayoutEvent(Core::PreLayoutEvent&) override;
            void paintEvent(Core::PaintEvent&) override;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
