// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Audio tool.
        class AudioTool : public IToolWidget
        {
            FEATHER_TK_NON_COPYABLE(AudioTool);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            AudioTool();

        public:
            virtual ~AudioTool();

            static std::shared_ptr<AudioTool> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            void _widgetUpdate();

            FEATHER_TK_PRIVATE();
        };
    }
}
