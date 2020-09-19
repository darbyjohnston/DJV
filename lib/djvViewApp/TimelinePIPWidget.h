// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/FrameNumber.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;

        } // namespace System
    } // namespace File

    namespace Render2D
    {
        class ImageOptions;

    } // namespace Render2D

    namespace ViewApp
    {
        class Media;

        class TimelinePIPWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelinePIPWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TimelinePIPWidget();

        public:
            ~TimelinePIPWidget() override;

            static std::shared_ptr<TimelinePIPWidget> create(const std::shared_ptr<System::Context>&);

            void setFileInfo(const System::File::Info&);
            void setPos(const glm::vec2&, Math::Frame::Index, const Math::BBox2f&);

            void setImageOptions(const Render2D::ImageOptions&);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

        protected:
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv
