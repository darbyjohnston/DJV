// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/MDICanvas.h>

namespace djv
{
    namespace ViewApp
    {
        class MediaWidget;

        //! This class provides a MDI canvas for media widgets.
        class MediaCanvas : public UI::MDI::Canvas
        {
            DJV_NON_COPYABLE(MediaCanvas);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MediaCanvas();

        public:
            ~MediaCanvas() override;

            static std::shared_ptr<MediaCanvas> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<MediaWidget> getActiveWidget() const;
            void setActiveCallback(const std::function<void(const std::shared_ptr<MediaWidget>&)>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

