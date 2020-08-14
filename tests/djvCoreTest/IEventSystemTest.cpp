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
        class TestEventSystem;

        class TestObject : public IObject
        {
            DJV_NON_COPYABLE(TestObject);
            
        protected:
            TestObject();

        public:
            static std::shared_ptr<TestObject> create(const std::shared_ptr<Context>&);
        };

        class TestObject2 : public TestObject
        {
            DJV_NON_COPYABLE(TestObject2);

        protected:
            TestObject2();

        public:
            bool event(Event::Event&) override;

        public:
            static std::shared_ptr<TestObject2> create(const std::shared_ptr<Context>&);

        protected:
            bool _eventFilter(const std::shared_ptr<Core::IObject>&, Core::Event::Event&) override;

        private:
            size_t _moveCount = 0;
            bool _buttonPress = false;
        };

        class TestEventSystem : public Event::IEventSystem
        {
            DJV_NON_COPYABLE(TestEventSystem);

            void _init(const std::shared_ptr<Context>&);

            TestEventSystem();

        public:
            static std::shared_ptr<TestEventSystem> create(const std::shared_ptr<Context>&);

            void tick() override;

        protected:
            void _addObject(const std::shared_ptr<TestObject>&);

            void _hover(const std::shared_ptr<IObject>&, Core::Event::PointerMove&, std::shared_ptr<Core::IObject>&);
            void _hover(Event::PointerMove&, std::shared_ptr<IObject>&) override;

        private:
            size_t _tick = 0;
            std::vector<std::weak_ptr<TestObject> > _objects;
            Event::PointerInfo _pointerInfo;

            friend class TestObject;
            friend class TestObject2;
        };

        TestObject::TestObject()
        {}

        std::shared_ptr<TestObject> TestObject::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TestObject>(new TestObject);
            out->_init(context);
            auto eventSystem = context->getSystemT<TestEventSystem>();
            eventSystem->_addObject(out);
            return out;
        }

        TestObject2::TestObject2()
        {}
            
        bool TestObject2::event(Event::Event& event)
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

        std::shared_ptr<TestObject2> TestObject2::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TestObject2>(new TestObject2);
            out->_init(context);
            auto eventSystem = context->getSystemT<TestEventSystem>();
            eventSystem->_addObject(out);
            return out;
        }
            
        bool TestObject2::_eventFilter(const std::shared_ptr<Core::IObject> &, Core::Event::Event &)
        {
            return false;
        }

        void TestEventSystem::_init(const std::shared_ptr<Context>& context)
        {
            IEventSystem::_init("TestEventSystem", context);
        }
            
        TestEventSystem::TestEventSystem()
        {}

        std::shared_ptr<TestEventSystem> TestEventSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TestEventSystem>(new TestEventSystem);
            out->_init(context);
            return out;
        }
            
        void TestEventSystem::tick()
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
            
        void TestEventSystem::_addObject(const std::shared_ptr<TestObject>& value)
        {
            _objects.push_back(value);
        }

        void TestEventSystem::_hover(const std::shared_ptr<IObject>& object, Core::Event::PointerMove& event, std::shared_ptr<Core::IObject>& hover)
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
            
        void TestEventSystem::_hover(Event::PointerMove& event, std::shared_ptr<IObject>& hover)
        {
            for (auto i = _objects.rbegin(); i != _objects.rend(); ++i)
            {
                if (auto object = i->lock())
                {
                    _hover(object, event, hover);
                    if (event.isAccepted())
                    {
                        break;
                    }
                }
            }
        }
        
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

                _clipboard();
                _textFocus();
                _tick();
                
                context->removeSystem(_system);
                _system.reset();
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

