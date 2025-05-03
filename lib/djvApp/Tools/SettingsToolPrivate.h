// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/SettingsTool.h>

#include <djvApp/Models/SettingsModel.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Advanced settings widget.
        class AdvancedSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(AdvancedSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            AdvancedSettingsWidget();

        public:
            virtual ~AdvancedSettingsWidget();

            static std::shared_ptr<AdvancedSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

            DTK_PRIVATE();
        };

        //! Cache settings widget.
        class CacheSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(CacheSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            CacheSettingsWidget();

        public:
            virtual ~CacheSettingsWidget();

            static std::shared_ptr<CacheSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        //! File browser settings widget.
        class FileBrowserSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(FileBrowserSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            FileBrowserSettingsWidget();

        public:
            virtual ~FileBrowserSettingsWidget();

            static std::shared_ptr<FileBrowserSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

            DTK_PRIVATE();
        };

        //! File sequence settings widget.
        class FileSequenceSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(FileSequenceSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            FileSequenceSettingsWidget();

        public:
            virtual ~FileSequenceSettingsWidget();

            static std::shared_ptr<FileSequenceSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        //! Miscellaneous settings widget.
        class MiscSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(MiscSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            MiscSettingsWidget();

        public:
            virtual ~MiscSettingsWidget();

            static std::shared_ptr<MiscSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        //! Mouse settings widget.
        class MouseSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(MouseSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            MouseSettingsWidget();

        public:
            virtual ~MouseSettingsWidget();

            static std::shared_ptr<MouseSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };

        //! Keyboard shortcut widget.
        class ShortcutWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(ShortcutWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            ShortcutWidget();

        public:
            virtual ~ShortcutWidget();

            static std::shared_ptr<ShortcutWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setShortcut(const Shortcut&);
            void setCallback(const std::function<void(const Shortcut&)>&);
            void setCollision(bool);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;
            void drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent&) override;
            void mouseEnterEvent(dtk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mousePressEvent(dtk::MouseClickEvent&) override;
            void keyFocusEvent(bool) override;
            void keyPressEvent(dtk::KeyEvent&) override;
            void keyReleaseEvent(dtk::KeyEvent&) override;

        private:
            void _widgetUpdate();

            DTK_PRIVATE();
        };

        //! Keyboard shortcuts settings widget.
        class ShortcutsSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(ShortcutsSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            ShortcutsSettingsWidget();

        public:
            virtual ~ShortcutsSettingsWidget();

            static std::shared_ptr<ShortcutsSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            void _widgetUpdate(const ShortcutsSettings&);

            DTK_PRIVATE();
        };

        //! Style settings widget.
        class StyleSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(StyleSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            StyleSettingsWidget();

        public:
            virtual ~StyleSettingsWidget();

            static std::shared_ptr<StyleSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            void _widgetUpdate(const StyleSettings&);

            DTK_PRIVATE();
        };

#if defined(TLRENDER_FFMPEG)
        //! FFmpeg settings widget.
        class FFmpegSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(FFmpegSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            FFmpegSettingsWidget();

        public:
            virtual ~FFmpegSettingsWidget();

            static std::shared_ptr<FFmpegSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };
#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
        //! USD settings widget.
        class USDSettingsWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(USDSettingsWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            USDSettingsWidget();

        public:
            virtual ~USDSettingsWidget();

            static std::shared_ptr<USDSettingsWidget> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };
#endif // TLRENDER_USD
    }
}
