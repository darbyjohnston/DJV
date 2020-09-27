// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUITest/SelectionModelTest.h>

#include <djvUI/SelectionModel.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;
using namespace djv::UI;

namespace djv
{
    namespace UITest
    {
        SelectionModelTest::SelectionModelTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITickTest("djv::UITest::SelectionModelTest", tempPath, context)
        {}
        
        void SelectionModelTest::run()
        {
            {
                auto selectionModel = SelectionModel::create(SelectionType::None);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->select(0);
                DJV_ASSERT(selected.empty());
                selectionModel->setCount(5);
                selectionModel->select(0);
                DJV_ASSERT(selected.empty());
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Single);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->select(0);
                DJV_ASSERT(selected.empty());
                selectionModel->setCount(5);
                selectionModel->select(0);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 0 }) == selected);
                selectionModel->select(1);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);
                selectionModel->select(1);
                DJV_ASSERT(selected.empty());
                selectionModel->select(1);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Radio);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->select(0);
                DJV_ASSERT(selected.empty());
                selectionModel->setCount(5);
                selectionModel->select(0);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 0 }) == selected);
                selectionModel->select(1);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);
                selectionModel->select(1);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Multiple);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->select(0);
                DJV_ASSERT(selected.empty());
                selectionModel->setCount(5);
                selectionModel->select(0);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 0 }) == selected);
                selectionModel->select(0);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 0 }) == selected);
                selectionModel->select(1);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);

                selectionModel->select(0, GLFW_MOD_SHIFT);
                DJV_ASSERT(2 == selected.size() && std::set<size_t>({ 0, 1 }) == selected);
                selectionModel->select(4, GLFW_MOD_SHIFT);
                DJV_ASSERT(4 == selected.size() && std::set<size_t>({ 1, 2, 3, 4 }) == selected);
                selectionModel->select(0);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 0 }) == selected);

                selectionModel->select(0, GLFW_MOD_CONTROL);
                DJV_ASSERT(selected.empty());
                selectionModel->select(1, GLFW_MOD_CONTROL);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);
                selectionModel->select(3, GLFW_MOD_CONTROL);
                DJV_ASSERT(2 == selected.size() && std::set<size_t>({ 1, 3 }) == selected);
                selectionModel->select(0, GLFW_MOD_SHIFT);
                DJV_ASSERT(4 == selected.size() && std::set<size_t>({ 0, 1, 2, 3 }) == selected);
                selectionModel->select(4, GLFW_MOD_SHIFT);
                DJV_ASSERT(2 == selected.size() && std::set<size_t>({ 3, 4 }) == selected);
                selectionModel->select(4);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 4 }) == selected);
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Multiple);

                DJV_ASSERT(0 == selectionModel->getCount());
                selectionModel->setCount(3);
                DJV_ASSERT(3 == selectionModel->getCount());

                const std::set<size_t> selected({ 0, 2 });
                selectionModel->setSelected(selected);
                DJV_ASSERT(selectionModel->getSelected() == selected);
                DJV_ASSERT(selectionModel->isSelected(0));
                DJV_ASSERT(!selectionModel->isSelected(1));
                DJV_ASSERT(selectionModel->isSelected(2));

                selectionModel->setCount(0);
                DJV_ASSERT(selectionModel->getSelected().empty());
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Multiple);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->setCount(3);
                selectionModel->setSelected({ 0, 2 });
                selectionModel->invertSelection();
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 1 }) == selected);

                selectionModel->selectAll();
                DJV_ASSERT(3 == selected.size() && std::set<size_t>({ 0, 1, 2 }) == selected);

                selectionModel->selectNone();
                DJV_ASSERT(selected.empty());
            }

            {
                auto selectionModel = SelectionModel::create(SelectionType::Multiple);

                std::set<size_t> selected;
                selectionModel->setCallback(
                    [&selected](const std::set<size_t>& value)
                    {
                        selected = value;
                    });

                selectionModel->setCount(3);
                selectionModel->select(2, GLFW_MOD_SHIFT);
                DJV_ASSERT(1 == selected.size() && std::set<size_t>({ 2 }) == selected);
            }
        }

    } // namespace UITest
} // namespace djv

