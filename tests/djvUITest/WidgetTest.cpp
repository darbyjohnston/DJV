// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUITest/WidgetTest.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ColorSwatch.h>
#include <djvUI/EventSystem.h>
#include <djvUI/IntSlider.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GridLayout.h>
#include <djvUI/IconWidget.h>
#include <djvUI/Label.h>
#include <djvUI/LineEdit.h>
#include <djvUI/ListButton.h>
#include <djvUI/PushButton.h>
#include <djvUI/ToggleButton.h>
#include <djvUI/ToolButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvSystem/Context.h>

#include <djvCore/RandomFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;
using namespace djv::UI;

namespace djv
{
    namespace UITest
    {
        class TestEventSystem : public EventSystem
        {
            DJV_NON_COPYABLE(TestEventSystem);
            
            void _init(const std::shared_ptr<System::Context>& context)
            {
                EventSystem::_init("TestEventSystem", context);
            }
            
            TestEventSystem()
            {}

        public:
            static std::shared_ptr<TestEventSystem> create(const std::shared_ptr<System::Context>& context)
            {
                auto out = context->getSystemT<TestEventSystem>();
                if (!out)
                {
                    out = std::shared_ptr<TestEventSystem>(new TestEventSystem);
                    out->_init(context);
                }
                return out;
            }
            
            void tick() override
            {
                EventSystem::tick();
                
                const glm::vec2 size(1280, 720);
                
                for (const auto & i : _getWindows())
                {
                    if (auto window = i.lock())
                    {
                        window->resize(size);

                        System::Event::PreLayout preLayout;
                        _preLayoutRecursive(window, preLayout);

                        if (window->isVisible())
                        {
                            System::Event::Layout layout;
                            _layoutRecursive(window, layout);

                            System::Event::Clip clip(Math::BBox2f(0.F, 0.F, size.x, size.y));
                            _clipRecursive(window, clip);
                        }
                    }
                }
                
                for (const auto & i : _getWindows())
                {
                    if (auto window = i.lock())
                    {
                        if (window->isVisible())
                        {
                            System::Event::Paint paintEvent(Math::BBox2f(0.F, 0.F, size.x, size.y));
                            System::Event::PaintOverlay paintOverlayEvent(Math::BBox2f(0.F, 0.F, size.x, size.y));
                            _paintRecursive(window, paintEvent, paintOverlayEvent);
                        }
                    }
                }
                
                _pointerMove(_pointerInfo);
                _pointerInfo.projectedPos.x += Random::getRandom(1.f);
                _pointerInfo.projectedPos.y += Random::getRandom(1.f);
                switch (_tick)
                {
                case 1:
                    _buttonPress(0);
                    break;
                case 5:
                    _buttonRelease(0);
                    break;
                case 10:
                    _drop({ "one", "two", "three" });
                    break;
                case 15:
                    _keyPress(GLFW_KEY_TAB, 0);
                    break;
                case 20:
                    _keyRelease(GLFW_KEY_TAB, 0);
                    break;
                case 25:
                    _text(std::basic_string<djv_char_t>(), 0);
                    break;
                case 30:
                    _scroll(0.f, 0.f);
                    break;
                }
                ++_tick;
            }
            
        protected:
            void _hover(const std::shared_ptr<System::IObject>& object, System::Event::PointerMove& event, std::shared_ptr<System::IObject>& hover)
            {
                const auto children = object->getChildrenT<System::IObject>();
                for (auto i = children.rbegin(); i != children.rend(); ++i)
                {
                    _hover(*i, event, hover);
                    if (event.isAccepted())
                    {
                        break;
                    }
                }
                if (!event.isAccepted())
                {
                    object->event(event);
                    if (event.isAccepted())
                    {
                        hover = object;
                    }
                }
            }
            
            void _hover(System::Event::PointerMove& event, std::shared_ptr<System::IObject>& hover) override
            {
                const auto& windows = _getWindows();
                for (auto i = windows.rbegin(); i != windows.rend(); ++i)
                {
                    if (auto window = i->lock())
                    {
                        _hover(window, event, hover);
                        if (event.isAccepted())
                        {
                            break;
                        }
                    }
                }
            }
                    
        private:
            size_t _tick = 0;
            System::Event::PointerInfo _pointerInfo;
        };
        
        WidgetTest::WidgetTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITickTest("djv::UITest::WidgetTest", tempPath, context)
        {}
        
        void WidgetTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);
                auto widget = PushButton::create(context);
                    
                auto widget2 = ListButton::create(context);
                widget2->setHAlign(HAlign::Left);
                widget2->setHAlign(HAlign::Left);
                auto widget3 = ToolButton::create(context);
                widget3->setHAlign(HAlign::Right);
                auto widget4 = ToggleButton::create(context);
                widget4->setHAlign(HAlign::Center);
                    
                auto widget5 = Numeric::IntSlider::create(context);
                widget5->setVAlign(VAlign::Top);
                widget5->setVAlign(VAlign::Top);
                auto widget6 = Numeric::FloatSlider::create(context);
                widget6->setVAlign(VAlign::Bottom);
                auto widget7 = Text::LineEdit::create(context);
                widget7->setVAlign(VAlign::Center);
                    
                auto stackLayout = StackLayout::create(context);
                stackLayout->addChild(widget);
                stackLayout->addChild(widget2);
                stackLayout->addChild(widget3);
                stackLayout->addChild(widget4);
                stackLayout->addChild(widget5);
                stackLayout->addChild(widget6);
                    
                auto rowLayout = HorizontalLayout::create(context);
                auto label = Text::Label::create(context);
                label->setText("Hello world!");
                rowLayout->addChild(label);
                auto iconWidget = IconWidget::create(context);
                iconWidget->setIcon("djvIconFileOpen");
                rowLayout->addChild(iconWidget);

                auto formLayout = FormLayout::create(context);
                formLayout->addChild(CheckBox::create(context));
                formLayout->addChild(ColorSwatch::create(context));

                auto gridLayout = GridLayout::create(context);
                gridLayout->addChild(stackLayout);
                gridLayout->addChild(rowLayout);
                gridLayout->setGridPos(rowLayout, 1, 0);
                gridLayout->addChild(formLayout);
                gridLayout->setGridPos(formLayout, 0, 1);

                auto window = Window::create(context);
                window->addChild(gridLayout);
                if (auto parentWindow = gridLayout->getWindow())
                {
                    DJV_ASSERT(window == parentWindow);
                }
                window->show();
                    
                _tickFor(std::chrono::milliseconds(1000));
                    
                window->close();
            }
        }

    } // namespace UITest
} // namespace djv

