// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/SelectionModel.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

namespace djv
{
    namespace UI
    {
        SelectionModel::SelectionModel(SelectionType selectionType) :
            _selectionType(selectionType)
        {}

        SelectionModel::~SelectionModel()
        {}

        std::shared_ptr<SelectionModel> SelectionModel::create(SelectionType selectionType)
        {
            auto out = std::shared_ptr<SelectionModel>(new SelectionModel(selectionType));
            return out;
        }

        void SelectionModel::setCount(size_t value)
        {
            _count = value;
            _anchor = -1;
            if (!_selected.empty())
            {
                _selected.clear();
                if (_callback)
                {
                    _callback(_selected);
                }
            }
        }

        void SelectionModel::setSelected(const std::set<size_t>& value)
        {
            _selected = value;
        }

        void SelectionModel::selectAll()
        {
            std::set<size_t> selected;
            for (size_t i = 0; i < _count; ++i)
            {
                selected.insert(i);
            }
            if (selected != _selected)
            {
                _selected = selected;
                if (_callback)
                {
                    _callback(_selected);
                }
            }
        }

        void SelectionModel::selectNone()
        {
            if (!_selected.empty())
            {
                _selected.clear();
                if (_callback)
                {
                    _callback(_selected);
                }
            }
        }

        void SelectionModel::invertSelection()
        {
            std::set<size_t> selected;
            for (size_t i = 0; i < _count; ++i)
            {
                if (_selected.find(i) == _selected.end())
                {
                    selected.insert(i);
                }
            }
            if (selected != _selected)
            {
                _selected = selected;
                if (_callback)
                {
                    _callback(_selected);
                }
            }
        }

        void SelectionModel::select(size_t index, int keyModifiers)
        {
            if (index < _count)
            {
                std::set<size_t> selected = _selected;
                switch (_selectionType)
                {
                case SelectionType::Single:
                {
                    const bool select = selected.find(index) == selected.end();
                    selected.clear();
                    if (select)
                    {
                        selected.insert(index);
                    }
                    break;
                }
                case SelectionType::Radio:
                    selected.clear();
                    selected.insert(index);
                    break;
                case SelectionType::Multiple:
                {
                    const auto i = selected.find(index);
                    if (keyModifiers & GLFW_MOD_SHIFT)
                    {
                        selected.clear();
                        if (-1 == _anchor)
                        {
                            _anchor = index;
                            selected.insert(index);
                        }
                        else
                        {
                            if (_anchor < index)
                            {
                                for (size_t i = _anchor; i <= index; ++i)
                                {
                                    selected.insert(i);
                                }
                            }
                            else
                            {
                                for (size_t i = index; i <= _anchor; ++i)
                                {
                                    selected.insert(i);
                                }
                            }
                        }
                    }
                    else if (keyModifiers & GLFW_MOD_CONTROL)
                    {
                        _anchor = index;
                        if (i != selected.end())
                        {
                            selected.erase(i);
                        }
                        else
                        {
                            selected.insert(index);
                        }
                    }
                    else
                    {
                        _anchor = index;
                        selected.clear();
                        selected.insert(index);
                    }
                    break;
                }
                default: break;
                }
                if (selected != _selected)
                {
                    _selected = selected;
                    if (_callback)
                    {
                        _callback(_selected);
                    }
                }
            }
        }

        void SelectionModel::setCallback(const std::function<void(const std::set<size_t>&)>& value)
        {
            _callback = value;
        }


    } // namespace UI
} // namespace Gp
