// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        inline size_t SelectionModel::getCount() const
        {
            return _count;
        }

        inline const std::set<size_t>& SelectionModel::getSelected() const
        {
            return _selected;
        }

        inline bool SelectionModel::isSelected(size_t value) const
        {
            return _selected.find(value) != _selected.end();
        }

    } // namespace UI
} // namespace Gp
