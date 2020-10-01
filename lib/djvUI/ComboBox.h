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
        //! \todo When the combo box is opened, position the current item under the pointer.
        class ComboBox : public Widget
        {
            DJV_NON_COPYABLE(ComboBox);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ComboBox();

        public:
            ~ComboBox() override;

            static std::shared_ptr<ComboBox> create(const std::shared_ptr<System::Context>&);

            //! \name Items
            ///@{

            void setItems(const std::vector<std::string>&);
            void setItems(const std::vector<std::shared_ptr<Action> >&);
            void clearItems();

            int getCurrentItem() const;

            void setCurrentItem(int);
            void firstItem();
            void lastItem();
            void prevItem();
            void nextItem();

            ///@}

            //! \name Open and Close
            ///@{

            bool isOpen() const;

            void open();
            void close();

            ///@}

            //! \name Text Font
            ///@{

            void setFont(int, const std::string&);
            void setFontSizeRole(MetricsRole);

            ///@}

            //! \name Callbacks
            ///@{

            void setCallback(const std::function<void(int)>&);

            ///@}

            std::shared_ptr<Widget> getFocusWidget() override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _keyPressEvent(System::Event::KeyPress&) override;

        private:
            void _itemsUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

