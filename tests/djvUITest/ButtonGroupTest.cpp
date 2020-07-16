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
                    auto buttonGroup = ButtonGroup::create(ButtonType::Radio);
                    DJV_ASSERT(buttonGroup->getButtons().empty());
                    DJV_ASSERT(0 == buttonGroup->getButtonCount());
                    DJV_ASSERT(-1 == buttonGroup->getButtonIndex(nullptr));
                    DJV_ASSERT(ButtonType::Radio == buttonGroup->getButtonType());
                    DJV_ASSERT(-1 == buttonGroup->getChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Radio);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttonGroup->addButton(buttons[0]);
                    buttonGroup->addButton(buttons[1]);
                    buttonGroup->addButton(buttons[2]);
                    DJV_ASSERT(3 == buttonGroup->getButtonCount());
                    DJV_ASSERT(0 == buttonGroup->getButtonIndex(buttons[0]));
                    DJV_ASSERT(1 == buttonGroup->getButtonIndex(buttons[1]));
                    DJV_ASSERT(2 == buttonGroup->getButtonIndex(buttons[2]));
                    buttonGroup->removeButton(buttons[1]);
                    DJV_ASSERT(2 == buttonGroup->getButtonCount());
                    DJV_ASSERT(0 == buttonGroup->getButtonIndex(buttons[0]));
                    DJV_ASSERT(1 == buttonGroup->getButtonIndex(buttons[2]));
                    buttonGroup->clearButtons();
                    DJV_ASSERT(0 == buttonGroup->getButtonCount());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Push);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context),
                    };
                    buttonGroup->addButton(buttons[0]);
                    buttonGroup->addButton(buttons[1]);
                    buttonGroup->setChecked(0);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                    buttonGroup->setChecked(1);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Toggle);
                    bool buttonChecked[2] = { false, false };
                    buttonGroup->setToggleCallback(
                        [&buttonChecked](int index, bool value)
                        {
                            buttonChecked[index] = value;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttonGroup->addButton(buttons[0]);
                    buttonGroup->addButton(buttons[1]);
                    buttonGroup->setChecked(0);
                    DJV_ASSERT(buttonChecked[0]);
                    DJV_ASSERT(!buttonChecked[1]);
                    DJV_ASSERT(buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                    buttonGroup->setChecked(1);
                    DJV_ASSERT(buttonChecked[1]);
                    DJV_ASSERT(buttons[0]->isChecked());
                    DJV_ASSERT(buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Radio);
                    int buttonChecked = -1;
                    buttonGroup->setRadioCallback(
                        [&buttonChecked](int index)
                        {
                            buttonChecked = index;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttonGroup->addButton(buttons[0]);
                    buttonGroup->addButton(buttons[1]);
                    DJV_ASSERT(0 == buttonChecked);
                    DJV_ASSERT(buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                    buttonGroup->setChecked(1);
                    DJV_ASSERT(1 == buttonChecked);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(buttons[1]->isChecked());
                    buttonGroup->removeButton(buttons[1]);
                    DJV_ASSERT(0 == buttonChecked);
                    DJV_ASSERT(buttons[0]->isChecked());
                    buttonGroup->removeButton(buttons[0]);
                    DJV_ASSERT(-1 == buttonChecked);
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Exclusive);
                    int buttonChecked = -1;
                    buttonGroup->setExclusiveCallback(
                        [&buttonChecked](int index)
                        {
                            buttonChecked = index;
                        });
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttonGroup->addButton(buttons[0]);
                    buttonGroup->addButton(buttons[1]);
                    buttonGroup->setChecked(0);
                    DJV_ASSERT(0 == buttonChecked);
                    DJV_ASSERT(buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                    buttonGroup->setChecked(1);
                    DJV_ASSERT(1 == buttonChecked);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(buttons[1]->isChecked());
                    buttonGroup->setChecked(1, false);
                    DJV_ASSERT(-1 == buttonChecked);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Toggle);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttonGroup->setButtons(buttons, 1);
                    DJV_ASSERT(buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Radio);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttons[0]->setChecked(true);
                    buttons[1]->setChecked(true);
                    buttonGroup->setButtons(buttons, 1);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Exclusive);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttons[0]->setChecked(true);
                    buttons[1]->setChecked(true);
                    buttonGroup->setButtons(buttons, 1);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(buttons[1]->isChecked());
                }
                {
                    auto buttonGroup = ButtonGroup::create(ButtonType::Exclusive);
                    std::vector<std::shared_ptr<Button::IButton> > buttons =
                    {
                        PushButton::create(context),
                        PushButton::create(context)
                    };
                    buttons[0]->setChecked(true);
                    buttons[1]->setChecked(true);
                    buttonGroup->setButtons(buttons, -1);
                    DJV_ASSERT(!buttons[0]->isChecked());
                    DJV_ASSERT(!buttons[1]->isChecked());
                }
            }
        }

    } // namespace UITest
} // namespace djv

