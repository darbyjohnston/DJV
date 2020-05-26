// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides an I/O threads settings widget.
        class IOThreadsSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(IOThreadsSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            IOThreadsSettingsWidget();

        public:
            static std::shared_ptr<IOThreadsSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

