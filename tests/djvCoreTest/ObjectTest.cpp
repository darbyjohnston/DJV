//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvCoreTest/ObjectTest.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/IObject.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ObjectTest::ObjectTest(const std::shared_ptr<Context>& context) :
            ITest("djv::CoreTest::ObjectTest", context)
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
                void _hover(Event::PointerMove&, std::shared_ptr<IObject>&) override
                {}
            };
        
        } // namespace

        void ObjectTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = TestEventSystem::create(context);
                
                {
                    auto o = TestObject::create(context);
                    DJV_ASSERT(!o->getParent().lock());
                    DJV_ASSERT(o->getChildren().size() == 0);
                    DJV_ASSERT(o->isEnabled());
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

