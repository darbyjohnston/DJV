// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Image
    {
        class Data;

    } // namespace Image

    namespace ViewApp
    {
        //! This class provides the image system.
        class ImageSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(ImageSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ImageSystem();

        public:
            ~ImageSystem() override;

            static std::shared_ptr<ImageSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFrameStoreEnabled() const;
            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Image::Data> > > observeFrameStoreImage() const;

            void setFrameStoreEnabled(bool);
            void loadFrameStore();
            void clearFrameStore();

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<MenuData> getMenuData() const override;
            std::vector<ActionData> getToolActionData() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            void deleteToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            void _actionsUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

