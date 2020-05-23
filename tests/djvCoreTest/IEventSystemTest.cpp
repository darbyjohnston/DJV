// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/IEventSystemTest.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/IObject.h>
#include <djvCore/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        class TestObject : public IObject
        {
            DJV_NON_COPYABLE(TestObject);
            
        protected:
            void _init(const std::shared_ptr<Context>& context)
            {
                IObject::_init(context);
            }
            
            TestObject()
            {}

        public:
            static std::shared_ptr<TestObject> create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<TestObject>(new TestObject);
                out->_init(context);
                return out;
            }
        };

        class TestObject2 : public TestObject
        {
            DJV_NON_COPYABLE(TestObject2);

        protected:
            TestObject2()
            {}
            
        public:
            bool event(Event::Event& event) override
            {
                bool out = IObject::event(event);
                if (!out)
                {
                    switch (event.getEventType())
                    {
                    case Event::Type::PointerEnter:
                        event.accept();
                        break;
                    case Event::Type::PointerLeave:
                        event.accept();
                        break;
                    case Event::Type::PointerMove:
                        if (_buttonPress)
                        {
                            if (_moveCount < 3)
                            {
                                event.accept();
                            }
                            ++_moveCount;
                        }
                        else
                        {
                            event.accept();
                        }
                        break;
                    case Event::Type::ButtonPress:
                        event.accept();
                        _buttonPress = true;
                        break;
                    case Event::Type::ButtonRelease:
                        event.accept();
                        _buttonPress = false;
                        break;
                    case Event::Type::Drop:
                        event.accept();
                        break;
                    case Event::Type::KeyPress:
                        event.accept();
                        break;
                    case Event::Type::KeyRelease:
                        event.accept();
                        break;
                    case Event::Type::TextInput:
                        event.accept();
                        break;
                    case Event::Type::Scroll:
                        event.accept();
                        break;
                    default: break;
                    }
                    out = event.isAccepted();
                }
                return out;
            }

        public:
            static std::shared_ptr<TestObject2> create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<TestObject2>(new TestObject2);
                out->_init(context);
                return out;
            }
            
        protected:
            bool _eventFilter(const std::shared_ptr<Core::IObject> &, Core::Event::Event &) override
            {
                return false;
            }
            
        private:
            size_t _moveCount = 0;
            bool _buttonPress = false;
        };

        class TestEventSystem : public Event::IEventSystem
        {
            DJV_NON_COPYABLE(TestEventSystem);
            void _init(const std::shared_ptr<Context>& context)
            {
                IEventSystem::_init("TestEventSystem", context);
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
            
            void tick() override
            {
                IEventSystem::tick();
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
                const auto objects = rootObject->getChildrenT<TestObject>();
                for (auto i = objects.rbegin(); i != objects.rend(); ++i)
                {
                    _hover(*i, event, hover);
                    if (event.isAccepted())
                    {
                        break;
                    }                
                }
            }
            
            void _initObject(const std::shared_ptr<Core::IObject>& object) override
            {
                IEventSystem::_initObject(object);
                if (auto testObject = std::dynamic_pointer_cast<TestObject>(object))
                {
                    getRootObject()->addChild(testObject);
                }
            }
        
        private:
            size_t _tick = 0;
            Event::PointerInfo _pointerInfo;
        };

        IEventSystemTest::IEventSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::IEventSystemTest", context)
        {}
                
        void IEventSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                _system = TestEventSystem::create(context);
                
                _object = TestObject::create(context);
                _object2 = TestObject2::create(context);
                _object2->installEventFilter(_object);
                _object->addChild(_object2);

                _info();
                _clipboard();
                _textFocus();
                _tick();
                
                context->removeSystem(_system);
                _system.reset();
            }
        }
        
        void IEventSystemTest::_info()
        {
            {
                std::stringstream ss;
                ss << "root object class: " << _system->getRootObject()->getClassName();
                _print(ss.str());
            }
            {
                std::stringstream ss;
                ss << "root object name: " << _system->getRootObject()->getObjectName();
                _print(ss.str());
            }
        }
         
        void IEventSystemTest::_clipboard()
        {
            {
                std::stringstream ss;
                ss << "clipboard: " << _system->getClipboard();
                _print(ss.str());
            }
            _system->setClipboard(std::string());
        }
                  
        void IEventSystemTest::_textFocus()
        {
            if (auto context = getContext().lock())
            {
                DJV_ASSERT(!_system->getTextFocus().lock());
                _system->setTextFocus(_object);
                _system->setTextFocus(_object);
                DJV_ASSERT(_object == _system->getTextFocus().lock());
                _system->setTextFocus(_object2);
                DJV_ASSERT(_object2 == _system->getTextFocus().lock());
            }
        }
         
        void IEventSystemTest::_tick()
        {
            if (auto context = getContext().lock())
            {
                _pointerObserver = ValueObserver<Event::PointerInfo>::create(
                    _system->observePointer(),
                    [this](const Event::PointerInfo& value)
                    {
                        std::stringstream ss;
                        ss << "pointer: " << value.projectedPos;
                        _print(ss.str());
                    });
                _hoverObserver = ValueObserver<std::shared_ptr<IObject> >::create(
                    _system->observeHover(),
                    [this](const std::shared_ptr<IObject>& value)
                    {
                        if (value)
                        {
                            std::stringstream ss;
                            ss << "hover: " << value->getClassName();
                            _print(ss.str());
                        }
                    });
                _grabObserver = ValueObserver<std::shared_ptr<IObject> >::create(
                    _system->observeGrab(),
                    [this](const std::shared_ptr<IObject>& value)
                    {
                        if (value)
                        {
                            std::stringstream ss;
                            ss << "grab: " << value->getClassName();
                            _print(ss.str());
                        }
                    });
                _keyGrabObserver = ValueObserver<std::shared_ptr<IObject> >::create(
                    _system->observeKeyGrab(),
                    [this](const std::shared_ptr<IObject>& value)
                    {
                        if (value)
                        {
                            std::stringstream ss;
                            ss << "key grab: " << value->getClassName();
                            _print(ss.str());
                        }
                    });

                _tickFor(std::chrono::milliseconds(2000));

                _object2->removeEventFilter(_object);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

