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

#include <djvCoreTest/ObjectTest.h>

#include <djvCore/IObject.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        ObjectTest::ObjectTest(Context * context) :
            ITest("djv::CoreTest::ObjectTest", context)
        {}
        
        namespace
        {
            class Object : public IObject
            {
                DJV_NON_COPYABLE(Object);

            protected:
                void _init(Context * context)
                {
                    IObject::_init(context);
                }

                Object()
                {}

            public:
                static std::shared_ptr<Object> create(Context * context)
                {
                    auto out = std::shared_ptr<Object>(new Object);
                    out->_init(context);
                    return out;
                }
            };
        
        } // namespace

        void ObjectTest::run(int & argc, char ** argv)
        {
            auto context = getContext();
            {
                auto o = Object::create(context);
                DJV_ASSERT(!o->getParent().lock());
                DJV_ASSERT(o->getChildren().size() == 0);
                DJV_ASSERT(o->isEnabled());
            }
            {
                auto parent = Object::create(context);
                auto child = Object::create(context);
                parent->addChild(child);
                DJV_ASSERT(child->getParent().lock() == parent);
                DJV_ASSERT(parent->getChildren().size() == 1 && parent->getChildren()[0] == child);
                parent->removeChild(child);
                DJV_ASSERT(parent->getChildren().size() == 0);
                DJV_ASSERT(!child->getParent().lock());
                parent->addChild(child);
                parent->clearChildren();
                DJV_ASSERT(parent->getChildren().size() == 0);
                DJV_ASSERT(!child->getParent().lock());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

