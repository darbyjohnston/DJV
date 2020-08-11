// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a search box widget.
        class SearchBox : public Widget
        {
            DJV_NON_COPYABLE(SearchBox);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SearchBox();

        public:
            ~SearchBox() override;

            static std::shared_ptr<SearchBox> create(const std::shared_ptr<Core::Context>&);

            const std::string & getFilter() const;
            void setFilter(const std::string &);
            void clearFilter();
            void setFilterCallback(const std::function<void(const std::string &)>&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

        private:
            void _doFilterCallback();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

