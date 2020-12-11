// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvImage/Type.h>

#include <djvAudio/Type.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

namespace djv
{
    namespace Image
    {
        class Size;
    
    } // namespace Image

    namespace ViewApp
    {
        //! This class provides the information widget.
        class InfoWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(InfoWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            InfoWidget();

        public:
            ~InfoWidget() override;

            static std::shared_ptr<InfoWidget> create(const std::shared_ptr<System::Context>&);

            void setFilter(const std::string&);

        protected:
            void _initLayoutEvent(djv::System::Event::InitLayout&) override;
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            std::string _text(int) const;
            std::string _text(const Math::Rational&) const;
            std::string _text(const Math::Frame::Sequence&, const Math::Rational&) const;
            std::string _text(Image::Type) const;
            std::string _text(const Image::Size&) const;
            std::string _text(Audio::Type) const;
            std::string _textSampleRate(size_t) const;
            std::string _textDuration(size_t, size_t) const;

            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

