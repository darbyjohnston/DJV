// Copyright (c) 2017-2018 Darby Johnston
// All rights reserved.
//
// See LICENSE.txt for licensing information.

#pragma once

#include <djvUI/Enum.h>

#include <functional>

namespace djv
{
    namespace UI
    {
        class IButton;

        //! This class manages a group of buttons.
        class ButtonGroup : public std::enable_shared_from_this<ButtonGroup>
        {
            DJV_NON_COPYABLE(ButtonGroup);

        protected:
            ButtonGroup();

        public:
            ~ButtonGroup();

            static std::shared_ptr<ButtonGroup> create(ButtonType);

            const std::vector<std::shared_ptr<IButton> >& getButtons() const;
            size_t getButtonCount() const;
            int getButtonIndex(const std::shared_ptr<IButton>&) const;
            void addButton(const std::shared_ptr<IButton>&);
            void removeButton(const std::shared_ptr<IButton>&);
            void clearButtons();

            ButtonType getButtonType() const;
            void setButtonType(ButtonType);

            int getChecked() const;
            void setChecked(int, bool = true);

            void setClickedCallback(const std::function<void(int)>&);
            void setCheckedCallback(const std::function<void(int, bool)>&);
            void setRadioCallback(const std::function<void(int)>&);

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
