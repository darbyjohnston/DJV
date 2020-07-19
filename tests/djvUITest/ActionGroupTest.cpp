// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUITest/ActionGroupTest.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/EventSystem.h>

using namespace djv::Core;
using namespace djv::UI;

namespace djv
{
    namespace UITest
    {
        class TestEventSystem : public EventSystem
        {
            DJV_NON_COPYABLE(TestEventSystem);
            void _init(const std::shared_ptr<Context>& context)
            {
                EventSystem::_init("TestEventSystem", context);
            }

            TestEventSystem()
            {}

        public:
            static std::shared_ptr<TestEventSystem> create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<TestEventSystem>(new TestEventSystem);
                out->_init(context);
                return out;
            }

        protected:
            void _hover(Event::PointerMove&, std::shared_ptr<IObject>&) override
            {}
        };

        ActionGroupTest::ActionGroupTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::UITest::ActionGroupTest", context)
        {}
        
        void ActionGroupTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);
                {
                    auto group = ActionGroup::create(ButtonType::Radio);
                    DJV_ASSERT(group->getActions().empty());
                    DJV_ASSERT(0 == group->getActionCount());
                    DJV_ASSERT(-1 == group->getActionIndex(nullptr));
                    DJV_ASSERT(ButtonType::Radio == group->getButtonType());
                    DJV_ASSERT(-1 == group->getChecked());
                }
                {
                    auto group = ActionGroup::create(ButtonType::Radio);
                    std::vector<std::shared_ptr<Action> > actions =
                    {
                        Action::create(),
                        Action::create(),
                        Action::create()
                    };
                    group->setActions({
                        actions[0],
                        actions[1],
                        actions[2] });
                    DJV_ASSERT(3 == group->getActionCount());
                    DJV_ASSERT(0 == group->getActionIndex(actions[0]));
                    DJV_ASSERT(1 == group->getActionIndex(actions[1]));
                    DJV_ASSERT(2 == group->getActionIndex(actions[2]));
                    group->clearActions();
                    DJV_ASSERT(0 == group->getActionCount());
                }
                {
                    auto group = ActionGroup::create(ButtonType::Toggle);
                    bool checked[2] = { false, false };
                    group->setToggleCallback(
                        [&checked](int index, bool value)
                        {
                            checked[index] = value;
                        });
                    std::vector<std::shared_ptr<Action> > actions =
                    {
                        Action::create(),
                        Action::create()
                    };
                    group->setActions({ actions[0], actions[1] });
                    actions[0]->doClick();
                    DJV_ASSERT(checked[0]);
                    DJV_ASSERT(!checked[1]);
                    actions[1]->doClick();
                    DJV_ASSERT(checked[1]);
                }
                {
                    auto group = ActionGroup::create(ButtonType::Radio);
                    int checked = -1;
                    group->setRadioCallback(
                        [&checked](int index)
                        {
                            checked = index;
                        });
                    std::vector<std::shared_ptr<Action> > actions =
                    {
                        Action::create(),
                        Action::create()
                    };
                    group->setActions({ actions[0], actions[1] });
                    group->setChecked(0);
                    actions[1]->doClick();
                    DJV_ASSERT(1 == checked);
                }
                {
                    auto group = ActionGroup::create(ButtonType::Exclusive);
                    int checked = -1;
                    group->setExclusiveCallback(
                        [&checked](int index)
                        {
                            checked = index;
                        });
                    std::vector<std::shared_ptr<Action> > actions =
                    {
                        Action::create(),
                        Action::create()
                    };
                    group->setActions({ actions[0], actions[1] });
                    group->setChecked(0);
                    actions[1]->doClick();
                    DJV_ASSERT(1 == checked);
                    actions[1]->doClick();
                    DJV_ASSERT(-1 == checked);
                }
            }
        }

    } // namespace UITest
} // namespace djv

