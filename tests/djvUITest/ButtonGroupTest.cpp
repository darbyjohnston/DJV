// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUITest/ButtonGroupTest.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/EventSystem.h>
#include <djvUI/PushButton.h>

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

        ButtonGroupTest::ButtonGroupTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::UITest::ButtonGroupTest", context)
        {}
        
        void ButtonGroupTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);
                {
                    auto group = ButtonGroup::create(ButtonType::Radio);
                    DJV_ASSERT(group->getButtons().empty());
                    DJV_ASSERT(0 == group->getButtonCount());
                    DJV_ASSERT(-1 == group->getButtonIndex(nullptr));
                    DJV_ASSERT(ButtonType::Radio == group->getButtonType());
                    DJV_ASSERT(-1 == group->getChecked());
                }
                {
                    auto group = ButtonGroup::create(ButtonType::Radio);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    group->setButtons({
                        buttons[0],
                        buttons[1],
                        buttons[2] });
                    DJV_ASSERT(3 == group->getButtonCount());
                    DJV_ASSERT(0 == group->getButtonIndex(buttons[0]));
                    DJV_ASSERT(1 == group->getButtonIndex(buttons[1]));
                    DJV_ASSERT(2 == group->getButtonIndex(buttons[2]));
                    group->clearButtons();
                    DJV_ASSERT(0 == group->getButtonCount());
                }
                {
                    auto group = ButtonGroup::create(ButtonType::Push);
                    int clicked = -1;
                    group->setPushCallback(
                        [&clicked](int value)
                        {
                            clicked = value;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    group->setButtons({ buttons[0], buttons[1] });
                    buttons[0]->doClick();
                    DJV_ASSERT(0 == clicked);
                    buttons[1]->doClick();
                    DJV_ASSERT(1 == clicked);
                }
                {
                    auto group = ButtonGroup::create(ButtonType::Toggle);
                    bool checked[2] = { false, false };
                    group->setToggleCallback(
                        [&checked](int index, bool value)
                        {
                            checked[index] = value;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    group->setButtons({ buttons[0], buttons[1] });
                    buttons[1]->doClick();
                    DJV_ASSERT(checked[1]);
                }
                {
                    auto group = ButtonGroup::create(ButtonType::Radio);
                    int checked = -1;
                    group->setRadioCallback(
                        [&checked](int index)
                        {
                            checked = index;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    group->setButtons({ buttons[0], buttons[1] });
                    buttons[1]->doClick();
                    DJV_ASSERT(1 == checked);
                    buttons[0]->doClick();
                    DJV_ASSERT(0 == checked);
                }
                {
                    auto group = ButtonGroup::create(ButtonType::Exclusive);
                    int checked = -1;
                    group->setExclusiveCallback(
                        [&checked](int index)
                        {
                            checked = index;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    group->setButtons({ buttons[0], buttons[1] });
                    buttons[1]->doClick();
                    DJV_ASSERT(1 == checked);
                    buttons[1]->doClick();
                    DJV_ASSERT(-1 == checked);
                }
            }
        }

    } // namespace UITest
} // namespace djv

