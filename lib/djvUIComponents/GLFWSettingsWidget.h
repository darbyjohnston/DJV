// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a GLFW settings widget.
        class GLFWSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(GLFWSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            GLFWSettingsWidget();

        public:
            static std::shared_ptr<GLFWSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(System::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

