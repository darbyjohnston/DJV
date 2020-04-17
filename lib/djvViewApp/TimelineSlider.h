// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

#include <djvCore/Frame.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
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

        //! This class provides a timeline slider.
        class TimelineSlider : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelineSlider);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TimelineSlider();

        public:
            ~TimelineSlider() override;

            static std::shared_ptr<TimelineSlider> create(const std::shared_ptr<Core::Context>&);

            void setMedia(const std::shared_ptr<Media>&);
            void setInOutPointsEnabled(bool);
            void setInPoint(Core::Frame::Index);
            void setOutPoint(Core::Frame::Index);
            void setCacheEnabled(bool);
            void setCacheSequence(const Core::Frame::Sequence&);
            void setCachedFrames(const Core::Frame::Sequence&);
            void setImageOptions(const AV::Render2D::ImageOptions&);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

            void setCurrentFrameCallback(const std::function<void(Core::Frame::Index)>&);
            void setCurrentFrameDragCallback(const std::function<void(bool)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint &) override;
            void _pointerEnterEvent(Core::Event::PointerEnter &) override;
            void _pointerLeaveEvent(Core::Event::PointerLeave &) override;
            void _pointerMoveEvent(Core::Event::PointerMove &) override;
            void _buttonPressEvent(Core::Event::ButtonPress &) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease &) override;

            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update &) override;

        private:
            Core::Frame::Index _posToFrame(float) const;
            float _frameToPos(Core::Frame::Index) const;
            float _getFrameLength() const;
            float _getSecondLength() const;
            float _getMinuteLength() const;
            float _getHourLength() const;
            Core::BBox2f _getHandleGeometry() const;
            void _textUpdate();
            void _currentFrameUpdate();

            void _doCurrentFrameCallback();
            void _doCurrentFrameDragCallback(bool);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv
