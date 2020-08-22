// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/MDIWidget.h>

#include <djvAV/Audio.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the information widget.
        class InfoWidget : public MDIWidget
        {
            DJV_NON_COPYABLE(InfoWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            InfoWidget();

        public:
            ~InfoWidget() override;

            static std::shared_ptr<InfoWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _initLayoutEvent(djv::Core::Event::InitLayout&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            std::string _text(int) const;
            std::string _text(const Core::Math::Rational&) const;
            std::string _text(const Core::Frame::Sequence&, const Core::Math::Rational&) const;
            std::string _text(AV::Image::Type) const;
            std::string _text(const AV::Image::Size&) const;
            std::string _text(AV::Audio::Type) const;
            std::string _textSampleRate(size_t) const;
            std::string _textDuration(size_t, size_t) const;

            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

