// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UIComponents
    {
        //! This class provides a list view header widget.
        class ListViewHeader : public UI::Widget
        {
            DJV_NON_COPYABLE(ListViewHeader);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ListViewHeader();

        public:
            ~ListViewHeader() override;

            static std::shared_ptr<ListViewHeader> create(const std::shared_ptr<System::Context>&);

            //! \name Text
            ///@{

            void setText(const std::vector<std::string> &);

            ///@}

            //! \name Sorting
            ///@{

            size_t getSort() const;
            bool getReverseSort() const;

            void setSort(size_t);
            void setReverseSort(bool);
            void setSortCallback(const std::function<void(size_t, bool)> &);

            //! \name Split
            ///@{

            const std::vector<float> & getSplit() const;

            void setSplit(const std::vector<float> &);
            void setSplitCallback(const std::function<void(const std::vector<float> &)> &);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _sortUpdate();

            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv

