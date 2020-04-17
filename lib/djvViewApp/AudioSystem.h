// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the audio system.
        class AudioSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(AudioSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AudioSystem();

        public:
            ~AudioSystem() override;

            static std::shared_ptr<AudioSystem> create(const std::shared_ptr<Core::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _actionsUpdate();

            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

