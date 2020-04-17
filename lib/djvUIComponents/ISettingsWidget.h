// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        class LabelSizeGroup;

        //! This class provides the base functionality for settings widgets.
        class ISettingsWidget : public Widget
        {
            DJV_NON_COPYABLE(ISettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ISettingsWidget();

        public:
            ~ISettingsWidget() override = 0;

            virtual std::string getSettingsName() const = 0;
            virtual std::string getSettingsSortKey() const = 0;
            virtual std::string getSettingsGroup() const = 0;

            virtual void setSizeGroup(const std::weak_ptr<LabelSizeGroup>&) {}

            float getHeightForWidth(float) const override;

            void addChild(const std::shared_ptr<IObject> &) override;
            void removeChild(const std::shared_ptr<IObject> &) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

