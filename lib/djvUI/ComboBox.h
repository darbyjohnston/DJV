// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a combo box widget.
        //!
        //! \todo Add support for icons. Should we use actions instead of items?
        //! \todo When the combo box is opened, position the current item under the pointer.
        //! \todo Don't close the combo box when the current item is changed with a keyboard shortcut?
        class ComboBox : public Widget
        {
            DJV_NON_COPYABLE(ComboBox);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ComboBox();

        public:
            ~ComboBox() override;

            static std::shared_ptr<ComboBox> create(const std::shared_ptr<Core::Context>&);

            const std::vector<std::string>& getItems() const;
            void setItems(const std::vector<std::string>&);
            void addItem(const std::string&);
            void clearItems(Callback = Callback::Suppress);

            int getCurrentItem() const;
            void setCurrentItem(int, Callback = Callback::Suppress);
            void firstItem(Callback = Callback::Suppress);
            void lastItem(Callback = Callback::Suppress);
            void prevItem(Callback = Callback::Suppress);
            void nextItem(Callback = Callback::Suppress);

            bool isOpen() const;
            void open();
            void close();

            void setFont(int, const std::string&);
            void setFontSizeRole(MetricsRole);

            void setCallback(const std::function<void(int)>&);

            std::shared_ptr<Widget> getFocusWidget() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;

        private:
            void _updateItems();
            void _updateCurrentItem(Callback);

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

