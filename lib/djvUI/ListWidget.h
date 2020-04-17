// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a list widget.
        //!
        //! \todo Add support for icons. Should we use actions instead of items?
        //! \todo Keep the current item visible in the scroll widget.
        class ListWidget : public Widget
        {
            DJV_NON_COPYABLE(ListWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ListWidget();

        public:
            virtual ~ListWidget();

            static std::shared_ptr<ListWidget> create(const std::shared_ptr<Core::Context>&);

            const std::vector<std::string> & getItems() const;
            void setItems(const std::vector<std::string> &);
            void addItem(const std::string &);
            void clearItems(Callback = Callback::Suppress);

            int getCurrentItem() const;
            void setCurrentItem(int, Callback = Callback::Suppress);
            void firstItem(Callback = Callback::Suppress);
            void lastItem(Callback = Callback::Suppress);
            void prevItem(Callback = Callback::Suppress);
            void nextItem(Callback = Callback::Suppress);
            void setCallback(const std::function<void(int)> &);

            void setBorder(bool);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;

        private:
            void _updateItems();
            void _updateCurrentItem(Callback);

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

