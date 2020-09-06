// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ObjectTest.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/IObject.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ObjectTest::ObjectTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ObjectTest", tempPath, context)
        {}
        
        namespace
        {
            class TestObject : public IObject
            {
                DJV_NON_COPYABLE(TestObject);

            protected:
                void _init(const std::shared_ptr<Context>& context)
                {
                    IObject::_init(context);
                    
                    DJV_ASSERT(_getResourceSystem());
                    DJV_ASSERT(_getLogSystem());
                    DJV_ASSERT(_getTextSystem());

                    setClassName("TestObject");

                    _log(_getText("TestObject"));
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
            
            protected:
                void _init(Event::Init&) override {}
                void _update(Event::Update&) override {}
                void _hover(Event::PointerMove&, std::shared_ptr<IObject>&) override {}
            };
        
        } // namespace

        void ObjectTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);
                
                {
                    auto o = TestObject::create(context);
                    DJV_ASSERT(o->getContext().lock());
                    DJV_ASSERT(!o->getClassName().empty());
                    DJV_ASSERT(o->getObjectName().empty());
                    DJV_ASSERT(!o->getParent().lock());
                    DJV_ASSERT(o->getChildren().size() == 0);
                    DJV_ASSERT(o->isEnabled());
                    o->setObjectName("Object");
                    DJV_ASSERT("Object" == o->getObjectName());
                }
                
                {
                    auto parent = TestObject::create(context);
                    auto child = TestObject::create(context);
                    parent->addChild(child);
                    DJV_ASSERT(child->getParent().lock() == parent);
                    DJV_ASSERT(parent->getChildren().size() == 1 && parent->getChildren()[0] == child);

                    auto child2 = TestObject::create(context);
                    DJV_ASSERT(child2->isEnabled());
                    child2->setEnabled(false);
                    DJV_ASSERT(!child2->isEnabled());
                    parent->addChild(child2);
                    child2->moveToFront();
                    child2->moveToBack();
                    
                    {
                        std::stringstream ss;
                        ss << "global object count: " << IObject::getGlobalObjectCount();
                        _print(ss.str());
                    }
                    {
                        std::map<std::string, size_t> counts;
                        IObject::getObjectCounts(parent, counts);
                        for (const auto& i : counts)
                        {
                            std::stringstream ss;
                            ss << i.first << ": " << i.second;
                            _print(ss.str());                            
                        }
                    }

                    parent->removeChild(child);
                    DJV_ASSERT(parent->getChildren().size() == 1);
                    parent->removeChild(child2);
                    DJV_ASSERT(parent->getChildren().size() == 0);
                    DJV_ASSERT(!child->getParent().lock());
                    DJV_ASSERT(!child2->getParent().lock());
                    
                    parent->addChild(child);
                    parent->addChild(child2);
                    parent->clearChildren();
                    DJV_ASSERT(parent->getChildren().size() == 0);
                    DJV_ASSERT(!child->getParent().lock());
                    
                    parent->addChild(child);
                    child->addChild(child2);
                    parent->addChild(child2);
                    DJV_ASSERT(parent->getChildren().size() == 2);
                    DJV_ASSERT(child->getChildren().size() == 0);
                }

                context->removeSystem(system);
            }
            
            {
                std::stringstream ss;
                ss << "global object count: " << IObject::getGlobalObjectCount();
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

