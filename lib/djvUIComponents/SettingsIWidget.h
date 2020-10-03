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
        namespace Text
        {
            class LabelSizeGroup;

        } // namespace Text
    } // namespace UI

    namespace UIComponents
    {
        //! This namespace provides settings functionality.
        namespace Settings
        {
            //! This class provides the base functionality for settings widgets.
            class IWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(IWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                IWidget();

            public:
                ~IWidget() override = 0;

                virtual std::string getSettingsName() const;
                virtual std::string getSettingsSortKey() const = 0;
                virtual std::string getSettingsGroup() const = 0;

                virtual void setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>&) {}

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

