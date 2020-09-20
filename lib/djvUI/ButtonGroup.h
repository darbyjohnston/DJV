// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            class IButton;

            //! This class manages a group of buttons.
            class Group : public std::enable_shared_from_this<Group>
            {
                DJV_NON_COPYABLE(Group);

            protected:
                Group(ButtonType);

            public:
                ~Group();

                static std::shared_ptr<Group> create(ButtonType);

                ButtonType getButtonType() const;

                const std::vector<std::shared_ptr<IButton> >& getButtons() const;
                size_t getButtonCount() const;
                int getButtonIndex(const std::shared_ptr<IButton>&) const;

                void setButtons(const std::vector<std::shared_ptr<IButton> >&);
                void clearButtons();

                int getChecked() const;

                void setChecked(int, bool = true);

                void setPushCallback(const std::function<void(int)>&);
                void setToggleCallback(const std::function<void(int, bool)>&);
                void setRadioCallback(const std::function<void(int)>&);
                void setExclusiveCallback(const std::function<void(int)>&);

            private:
                DJV_PRIVATE();
            };

        } // namespace Button

        typedef Button::Group ButtonGroup;

    } // namespace UI
} // namespace djv
