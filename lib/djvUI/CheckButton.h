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
            //! This class provides a check button.
            class Check : public IButton
            {
                DJV_NON_COPYABLE(Check);

            protected:
                void _init(Core::Context *);
                Check();

            public:
                virtual ~Check();

                static std::shared_ptr<Check> create(Core::Context *);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _paintEvent(Core::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Check CheckButton;

    } // namespace UI
} // namespace djv
