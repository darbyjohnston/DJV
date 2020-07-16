// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This struct provides a list item.
        struct ListItem
        {
            ListItem();
            ListItem(const std::string& text);
            
            std::string icon;
            std::string rightIcon;
            std::string text;
            std::string rightText;
            ColorRole   colorRole = ColorRole::None;
            std::string tooltip;

            bool operator == (const ListItem&) const;
        };

        //! This class provides a list widget.
        //!
        //! \todo Keep the current item visible in the scroll widget.
        class ListWidget : public Widget
        {
            DJV_NON_COPYABLE(ListWidget);

        protected:
            void _init(ButtonType, const std::shared_ptr<Core::Context>&);
            ListWidget();

        public:
            ~ListWidget() override;

            static std::shared_ptr<ListWidget> create(ButtonType, const std::shared_ptr<Core::Context>&);

            void setItems(const std::vector<std::string>&, int checked = -1);
            void setItems(const std::vector<ListItem>&, int checked = -1);
            void addItem(const ListItem&);
            void addItem(const std::string&);
            void clearItems();

            virtual void setButtonType(ButtonType);

            int getChecked() const;
            void setChecked(int, bool = true);

            void setPushCallback(const std::function<void(int)>&);
            void setToggleCallback(const std::function<void(int, bool)>&);
            void setRadioCallback(const std::function<void(int)>&);
            void setExclusiveCallback(const std::function<void(int)>&);

            void setFilter(const std::string&);

            void setAlternateRowsRoles(ColorRole, ColorRole);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;

        private:
            void _updateItems(int checked);
            void _updateFilter();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

