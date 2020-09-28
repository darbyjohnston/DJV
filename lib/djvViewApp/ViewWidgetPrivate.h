// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewData.h>
#include <djvViewApp/ViewWidget.h>

#include <djvRender2D/FontSystem.h>

namespace djv
{
    namespace ViewApp
    {
        enum class Grid
        {
            Column,
            Row
        };

        typedef std::pair<Grid, int> GridPos;

        class GridOverlay : public UI::Widget
        {
            DJV_NON_COPYABLE(GridOverlay);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            GridOverlay();

        public:
            static std::shared_ptr<GridOverlay> create(const std::shared_ptr<System::Context>&);

            void setOptions(const GridOptions&);

            void setImagePosAndZoom(const glm::vec2&, float);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio, float aspectRatio, float pixelAspectRatio);
            void setImageBBox(const Math::BBox2f&);

        protected:
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init&) override;
            void _updateEvent(System::Event::Update&) override;

        private:
            Math::BBox2f _getViewportWorld() const;
            Math::BBox2f _getViewport() const;
            std::string _getLabel(const GridPos&) const;
            void _textCreate(const GridPos&);
            void _textUpdate();

            DJV_PRIVATE();
        };

        class HUDOverlay : public UI::Widget
        {
            DJV_NON_COPYABLE(HUDOverlay);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            HUDOverlay();

        public:
            ~HUDOverlay() override;

            static std::shared_ptr<HUDOverlay> create(const std::shared_ptr<System::Context>&);

            void setHUDFrame(const Math::BBox2f&);
            void setHUDData(const HUDData&);
            void setHUDOptions(const HUDOptions&);

        protected:
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init&) override;
            void _updateEvent(System::Event::Update&) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

