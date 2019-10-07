//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUITest/WidgetTest.h>

#include <djvUI/EventSystem.h>
#include <djvUI/Widget.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

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
            
            void tick(float dt) override
            {
                EventSystem::tick(dt);
                
                const glm::vec2 size(1280, 720);
                
                auto rootObject = getRootObject();
                const auto windows = rootObject->getChildrenT<UI::Window>();
                for (const auto & i : windows)
                {
                    i->resize(size);

                    Event::PreLayout preLayout;
                    _preLayoutRecursive(i, preLayout);

                    if (i->isVisible())
                    {
                        Event::Layout layout;
                        _layoutRecursive(i, layout);

                        Event::Clip clip(BBox2f(0.F, 0.F, size.x, size.y));
                        _clipRecursive(i, clip);
                    }
                }
                
                for (const auto & i : windows)
                {
                    if (i->isVisible())
                    {
                        Event::Paint paintEvent(BBox2f(0.F, 0.F, size.x, size.y));
                        Event::PaintOverlay paintOverlayEvent(BBox2f(0.F, 0.F, size.x, size.y));
                        _paintRecursive(i, paintEvent, paintOverlayEvent);
                    }
                }
                
                _pointerMove(_pointerInfo);
                _pointerInfo.projectedPos.x += Math::getRandom(1.f);
                _pointerInfo.projectedPos.y += Math::getRandom(1.f);
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
                    _keyPress(0, 0);
                    break;
                case 20:
                    _keyRelease(0, 0);
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
            void _hover(const std::shared_ptr<IObject>& object, Core::Event::PointerMove& event, std::shared_ptr<Core::IObject>& hover)
            {
                const auto children = object->getChildrenT<IObject>();
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
            
            void _hover(Event::PointerMove& event, std::shared_ptr<IObject>& hover) override
            {
                auto rootObject = getRootObject();
                const auto objects = rootObject->getChildrenT<Window>();
                for (auto i = objects.rbegin(); i != objects.rend(); ++i)
                {
                    _hover(*i, event, hover);
                    if (event.isAccepted())
                    {
                        break;
                    }                
                }
            }
                    
        private:
            size_t _tick = 0;
            Event::PointerInfo _pointerInfo;
        };
        
        WidgetTest::WidgetTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::UITest::WidgetTest", context)
        {}
        
        void WidgetTest::run(const std::vector<std::string>& args)
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);

                {
                    auto widget = UI::Widget::create(context);
                    
                    auto window = UI::Window::create(context);
                    window->addChild(widget);
                    if (auto parentWindow = widget->getWindow())
                    {
                        DJV_ASSERT(window == parentWindow);
                    }

                    window->show();
                    
                    _tickFor(std::chrono::milliseconds(1000));
                }
            }
        }

    } // namespace UITest
} // namespace djv

