// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ToolButton.h>

namespace djv
{
    namespace ViewApp
    {
        //! View options grid widget.
        class ViewOptionsGridWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewOptionsGridWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewOptionsGridWidget();

        public:
            ~ViewOptionsGridWidget() override;

            static std::shared_ptr<ViewOptionsGridWidget> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! View options HUD widget.
        class ViewOptionsHUDWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewOptionsHUDWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewOptionsHUDWidget();

        public:
            ~ViewOptionsHUDWidget() override;

            static std::shared_ptr<ViewOptionsHUDWidget> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! View options background widget.
        class ViewOptionsBackgroundWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewOptionsBackgroundWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewOptionsBackgroundWidget();

        public:
            ~ViewOptionsBackgroundWidget() override;

            static std::shared_ptr<ViewOptionsBackgroundWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! View options border widget.
        class ViewOptionsBorderWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewOptionsBorderWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewOptionsBorderWidget();

        public:
            ~ViewOptionsBorderWidget() override;

            static std::shared_ptr<ViewOptionsBorderWidget> create(const std::shared_ptr<System::Context>&);

            const std::shared_ptr<UI::ToolButton>& getEnabledButton() const;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! View options widget.
        class ViewOptionsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ViewOptionsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewOptionsWidget();

        public:
            ~ViewOptionsWidget() override;

            static std::shared_ptr<ViewOptionsWidget> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, bool> getBellowsState() const;

            void setBellowsState(const std::map<std::string, bool>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

