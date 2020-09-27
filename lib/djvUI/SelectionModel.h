// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <functional>
#include <memory>
#include <set>

namespace djv
{
    namespace UI
    {
        //! This class provides a selection model.
        class SelectionModel : public std::enable_shared_from_this<SelectionModel>
        {
            DJV_NON_COPYABLE(SelectionModel);

        protected:
            SelectionModel(SelectionType);

        public:
            ~SelectionModel();

            static std::shared_ptr<SelectionModel> create(SelectionType);

            size_t getCount() const;

            void setCount(size_t);

            const std::set<size_t>& getSelected() const;
            bool isSelected(size_t) const;

            void setSelected(const std::set<size_t>&);
            void selectAll();
            void selectNone();
            void invertSelection();

            void select(size_t, int keyModifiers = 0);

            void setCallback(const std::function<void(const std::set<size_t>&)>&);

        private:
            SelectionType _selectionType = SelectionType::First;
            size_t _count = 0;
            std::set<size_t> _selected;
            size_t _anchor = -1;
            std::function<void(const std::set<size_t>&)> _callback;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/SelectionModelInline.h>
