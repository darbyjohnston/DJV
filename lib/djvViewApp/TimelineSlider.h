// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

#include <djvMath/FrameNumber.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class Media;

        //! Timeline slider widget.
        class TimelineSlider : public UI::Widget
        {
            DJV_NON_COPYABLE(TimelineSlider);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TimelineSlider();

        public:
            ~TimelineSlider() override;

            static std::shared_ptr<TimelineSlider> create(const std::shared_ptr<System::Context>&);

            //! \name Media
            ///@{

            void setMedia(const std::shared_ptr<Media>&);

            ///@}

            //! \name In/Out Points
            ///@{

            void setInOutPointsEnabled(bool);
            void setInPoint(Math::Frame::Index);
            void setOutPoint(Math::Frame::Index);

            ///@}

            //! \name Cache
            ///@{

            void setCacheEnabled(bool);
            void setCacheSequence(const Math::Frame::Sequence&);
            void setCachedFrames(const Math::Frame::Sequence&);

            ///@}

            //! \name Callbacks
            ///@{
            
            void setCurrentFrameCallback(const std::function<void(Math::Frame::Index)>&);
            void setCurrentFrameDragCallback(const std::function<void(bool)>&);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint &) override;
            void _pointerEnterEvent(System::Event::PointerEnter &) override;
            void _pointerLeaveEvent(System::Event::PointerLeave &) override;
            void _pointerMoveEvent(System::Event::PointerMove &) override;
            void _buttonPressEvent(System::Event::ButtonPress &) override;
            void _buttonReleaseEvent(System::Event::ButtonRelease &) override;

            void _initEvent(System::Event::Init&) override;
            void _updateEvent(System::Event::Update &) override;

        private:
            bool _isPIPEnabled() const;
            Math::Frame::Index _posToFrame(float) const;
            float _frameToPos(Math::Frame::Index) const;
            float _getFrameLength() const;
            float _getSecondLength() const;
            float _getMinuteLength() const;
            float _getHourLength() const;
            Math::BBox2f _getHandleGeometry() const;
            void _textUpdate();
            void _currentFrameUpdate();
            void _showPIP(bool);

            void _doCurrentFrameCallback();
            void _doCurrentFrameDragCallback(bool);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv
