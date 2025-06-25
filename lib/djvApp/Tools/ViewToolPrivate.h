// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/ViewTool.h>

#include <tlTimeline/BackgroundOptions.h>
#include <tlTimeline/DisplayOptions.h>
#include <tlTimeline/ForegroundOptions.h>

namespace djv
{
    namespace app
    {
        class ViewOptionsWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(ViewOptionsWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            ViewOptionsWidget();

        public:
            virtual ~ViewOptionsWidget();

            static std::shared_ptr<ViewOptionsWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };

        class BackgroundWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(BackgroundWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            BackgroundWidget();

        public:
            virtual ~BackgroundWidget();

            static std::shared_ptr<BackgroundWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            void _optionsUpdate(const tl::timeline::BackgroundOptions&);

            FEATHER_TK_PRIVATE();
        };

        class OutlineWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(OutlineWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            OutlineWidget();

        public:
            virtual ~OutlineWidget();

            static std::shared_ptr<OutlineWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            void _optionsUpdate(const tl::timeline::BackgroundOptions&);

            FEATHER_TK_PRIVATE();
        };

        class GridWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(GridWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            GridWidget();

        public:
            virtual ~GridWidget();

            static std::shared_ptr<GridWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
