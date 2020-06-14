// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a list view header widget.
        class ListViewHeader : public Widget
        {
            DJV_NON_COPYABLE(ListViewHeader);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ListViewHeader();

        public:
            virtual ~ListViewHeader();

            static std::shared_ptr<ListViewHeader> create(const std::shared_ptr<Core::Context>&);

            void setText(const std::vector<std::string> &);

            size_t getSort() const;
            bool getReverseSort() const;
            void setSort(size_t);
            void setReverseSort(bool);
            void setSortCallback(const std::function<void(size_t, bool)> &);

            const std::vector<float> & getSplit() const;
            void setSplit(const std::vector<float> &);
            void setSplitCallback(const std::function<void(const std::vector<float> &)> &);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            void _sortUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

