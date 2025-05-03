// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! Audio actions.
        class AudioActions : public IActions
        {
            DTK_NON_COPYABLE(AudioActions);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

            AudioActions();

        public:
            ~AudioActions();

            static std::shared_ptr<AudioActions> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

        private:
            DTK_PRIVATE();
        };
    }
}
