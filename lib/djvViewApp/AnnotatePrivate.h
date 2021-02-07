// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

namespace djv
{
    namespace Image
    {
        class Color;
    
    } // namespace Image

    namespace ViewApp
    {
        class AnnotateToolBar : public UI::Widget
        {
            DJV_NON_COPYABLE(AnnotateToolBar);

        protected:
            void _init(
                std::map<std::string, std::shared_ptr<UI::Action> >&,
                const std::shared_ptr<System::Context>&);
            AnnotateToolBar();

        public:
            ~AnnotateToolBar() override;

            static std::shared_ptr<AnnotateToolBar> create(
                std::map<std::string, std::shared_ptr<UI::Action> >&,
                const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class AnnotateSummaryWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AnnotateSummaryWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AnnotateSummaryWidget();

        public:
            ~AnnotateSummaryWidget() override;

            static std::shared_ptr<AnnotateSummaryWidget> create(const std::shared_ptr<System::Context>&);

            void setAuthor(const std::string&);
            void setText(const std::string&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

        class AnnotateCommentWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AnnotateCommentWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AnnotateCommentWidget();

        public:
            ~AnnotateCommentWidget() override;

            static std::shared_ptr<AnnotateCommentWidget> create(const std::shared_ptr<System::Context>&);

            void setAuthor(const std::string&);
            void setSequence(const Math::Frame::Sequence&);
            void setSpeed(const Math::IntRational&);
            void setFrame(Math::Frame::Index);
            void setText(const std::string&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        class AnnotateWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AnnotateWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AnnotateWidget();

        public:
            ~AnnotateWidget() override;

            static std::shared_ptr<AnnotateWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

