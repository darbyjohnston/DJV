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
        namespace Button
        {
            //! This class provides a toggle button.
            class Toggle : public IButton
            {
                DJV_NON_COPYABLE(Toggle);

            protected:
                void _init(Core::Context *);
                Toggle();

            public:
                virtual ~Toggle();

                static std::shared_ptr<Toggle> create(Core::Context *);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _paintEvent(Core::Event::Paint&) override;

            private:
                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Button
    } // namespace UI
} // namespace djv
