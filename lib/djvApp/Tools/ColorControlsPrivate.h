// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/ColorControlsTool.h>

#include <djvApp/Models/OCIOModel.h>

namespace djv
{
    namespace app
    {
        class OCIOWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(OCIOWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            OCIOWidget();

        public:
            virtual ~OCIOWidget();

            static std::shared_ptr<OCIOWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        class LUTWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(LUTWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            LUTWidget();

        public:
            virtual ~LUTWidget();

            static std::shared_ptr<LUTWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        class ColorWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(ColorWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            ColorWidget();

        public:
            virtual ~ColorWidget();

            static std::shared_ptr<ColorWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        class LevelsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(LevelsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            LevelsWidget();

        public:
            virtual ~LevelsWidget();

            static std::shared_ptr<LevelsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        class EXRDisplayWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(EXRDisplayWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            EXRDisplayWidget();

        public:
            virtual ~EXRDisplayWidget();

            static std::shared_ptr<EXRDisplayWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        class SoftClipWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(SoftClipWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            SoftClipWidget();

        public:
            virtual ~SoftClipWidget();

            static std::shared_ptr<SoftClipWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };
    }
}
