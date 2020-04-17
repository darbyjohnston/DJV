// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/Frame.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace AV
    {
        namespace Render2D
        {
            class ImageOptions;

        } // namespace Render2D
    } // namespace AV

    namespace ViewApp
    {
        class Media;

        class TimelinePIPWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelinePIPWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TimelinePIPWidget();

        public:
            ~TimelinePIPWidget() override;

            static std::shared_ptr<TimelinePIPWidget> create(const std::shared_ptr<Core::Context>&);

            void setFileInfo(const Core::FileSystem::FileInfo&);
            void setPos(const glm::vec2&, Core::Frame::Index, const Core::BBox2f&);

            void setImageOptions(const AV::Render2D::ImageOptions&);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

        protected:
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv
