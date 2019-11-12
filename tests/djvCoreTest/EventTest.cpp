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

#include <djvCoreTest/EventTest.h>

#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <codecvt>
#include <locale>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        EventTest::EventTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::EventTest", context)
        {}
        
        void EventTest::run(const std::vector<std::string>& args)
        {
            _enum();
            _event();
            _familyEvent();
            _coreEvent();
            _renderEvent();
            _inputEvent();
        }

        void EventTest::_enum()
        {
            for (auto i : Event::getTypeEnums())
            {
                std::stringstream ss;
                ss << "event type string: " << i;
                _print(ss.str());
            }
            
            for (auto i : Event::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                Event::Type j = Event::Type::First;
                ss >> j;
                DJV_ASSERT(i == j);
            }
        }

        namespace
        {
            class TestEvent : public Event::Event
            {
            public:
                TestEvent() :
                    Event::Event(Core::Event::Type::Count)
                {}
            };

        } // namespace

        void EventTest::_event()
        {
            {
                TestEvent event;
                DJV_ASSERT(event.getEventType() == Core::Event::Type::Count);
                DJV_ASSERT(!event.isAccepted());
                event.setAccepted(true);
                event.setAccepted(true);
                DJV_ASSERT(event.isAccepted());
                event.setAccepted(false);
                DJV_ASSERT(!event.isAccepted());
                event.accept();
                DJV_ASSERT(event.isAccepted());
            }
        }

        namespace
        {
            class Object : public IObject
            {
                DJV_NON_COPYABLE(Object);
                Object()
                {}

            public:
                static std::shared_ptr<Object> create(const std::shared_ptr<Context>&)
                {
                    return std::shared_ptr<Object>(new Object);
                }
            };
        
        } // namespace

        void EventTest::_familyEvent()
        {
            if (auto context = getContext().lock())
            {
                {
                    auto prevParent = Object::create(context);
                    auto newParent = Object::create(context);
                    const Event::ParentChanged event(prevParent, newParent);
                    DJV_ASSERT(event.getEventType() == Event::Type::ParentChanged);
                    DJV_ASSERT(event.getPrevParent() == prevParent);
                    DJV_ASSERT(event.getNewParent() == newParent);
                }
                
                {
                    auto child = Object::create(context);
                    const Event::ChildAdded event(child);
                    DJV_ASSERT(event.getEventType() == Event::Type::ChildAdded);
                    DJV_ASSERT(event.getChild() == child);
                }
                
                {
                    auto child = Object::create(context);
                    const Event::ChildRemoved event(child);
                    DJV_ASSERT(event.getEventType() == Event::Type::ChildRemoved);
                    DJV_ASSERT(event.getChild() == child);
                }
                
                {
                    const Event::ChildOrder event;
                    DJV_ASSERT(event.getEventType() == Event::Type::ChildOrder);
                }
            }
        }

        void EventTest::_coreEvent()
        {
            {
                const Event::Init event;
                DJV_ASSERT(event.getEventType() == Event::Type::Init);
            }
            
            {
                const float t = 1.F;
                const float dt = 2.F;
                const Event::Update event(t, dt);
                DJV_ASSERT(event.getEventType() == Event::Type::Update);
                DJV_ASSERT(event.getTime() == t);
                DJV_ASSERT(event.getDeltaTime() == dt);
            }
        }

        void EventTest::_renderEvent()
        {
            {
                const Event::PreLayout event;
                DJV_ASSERT(event.getEventType() == Event::Type::PreLayout);
            }
            
            {
                const Event::Layout event;
                DJV_ASSERT(event.getEventType() == Event::Type::Layout);
            }
            
            {
                const BBox2f rect(1.F, 2.F, 3.F, 4.F);
                Event::Clip event(rect);
                DJV_ASSERT(event.getEventType() == Event::Type::Clip);
                DJV_ASSERT(event.getClipRect() == rect);
                const BBox2f rect2(4.F, 3.F, 2.F, 1.F);
                event.setClipRect(rect2);
                DJV_ASSERT(event.getClipRect() == rect2);
            }
            
            {
                const BBox2f rect(1.F, 2.F, 3.F, 4.F);
                Event::Paint event(rect);
                DJV_ASSERT(event.getEventType() == Event::Type::Paint);
                DJV_ASSERT(event.getClipRect() == rect);
                const BBox2f rect2(4.F, 3.F, 2.F, 1.F);
                event.setClipRect(rect2);
                DJV_ASSERT(event.getClipRect() == rect2);
            }
            
            {
                const BBox2f rect(1.F, 2.F, 3.F, 4.F);
                Event::PaintOverlay event(rect);
                DJV_ASSERT(event.getEventType() == Event::Type::PaintOverlay);
                DJV_ASSERT(event.getClipRect() == rect);
                const BBox2f rect2(4.F, 3.F, 2.F, 1.F);
                event.setClipRect(rect2);
                DJV_ASSERT(event.getClipRect() == rect2);
            }
        }

        namespace
        {
            class TestPointerEvent : public Event::IPointer
            {
            public:
                TestPointerEvent(const Core::Event::PointerInfo& info) :
                    IPointer(info, Core::Event::Type::Count)
                {}
            };

            class TestKeyEvent : public Event::IKey
            {
            public:
                TestKeyEvent(int key, int keyModifiers, const Core::Event::PointerInfo& info) :
                    IKey(key, keyModifiers, info, Core::Event::Type::Count)
                {}
            };

        } // namespace

        void EventTest::_inputEvent()
        {
            {
                const Event::PointerInfo info;
                DJV_ASSERT(info == info);
            }
            
            {
                const Event::PointerInfo info;
                TestPointerEvent event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::Count);
                DJV_ASSERT(!event.isRejected());
                event.setRejected(true);
                event.setRejected(true);
                DJV_ASSERT(event.isRejected());
                event.setRejected(false);
                DJV_ASSERT(!event.isRejected());
                event.reject();
                DJV_ASSERT(event.isRejected());
                DJV_ASSERT(event.getPointerInfo() == info);
            }
            
            {
                const Event::PointerInfo info;
                const Event::PointerEnter event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::PointerEnter);
            }
            
            {
                const Event::PointerInfo info;
                const Event::PointerLeave event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::PointerLeave);
            }
            
            {
                const Event::PointerInfo info;
                const Event::PointerMove event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::PointerMove);
            }
            
            {
                const Event::PointerInfo info;
                const Event::ButtonPress event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::ButtonPress);
            }
            
            {
                const Event::PointerInfo info;
                const Event::ButtonRelease event(info);
                DJV_ASSERT(event.getEventType() == Event::Type::ButtonRelease);
            }
            
            {
                const glm::vec2 delta(1.f, 2.f);
                const Event::PointerInfo info;
                const Event::Scroll event(delta, info);
                DJV_ASSERT(event.getEventType() == Event::Type::Scroll);
                DJV_ASSERT(event.getScrollDelta() == delta);
            }
            
            {
                const std::vector<std::string> paths = { "a", "b" };
                const Event::PointerInfo info;
                const Event::Drop event(paths, info);
                DJV_ASSERT(event.getEventType() == Event::Type::Drop);
                DJV_ASSERT(event.getDropPaths() == paths);
            }
            
            {
                const int key = 1;
                const int keyModifiers = 2;
                const Event::PointerInfo info;
                const TestKeyEvent event(key, keyModifiers, info);
                DJV_ASSERT(event.getEventType() == Event::Type::Count);
                DJV_ASSERT(event.getKey() == key);
                DJV_ASSERT(event.getKeyModifiers() == keyModifiers);
            }
            
            {
                const Event::PointerInfo info;
                const Event::KeyPress event(1, 2, info);
                DJV_ASSERT(event.getEventType() == Event::Type::KeyPress);
            }
            
            {
                const Event::PointerInfo info;
                const Event::KeyRelease event(1, 2, info);
                DJV_ASSERT(event.getEventType() == Event::Type::KeyRelease);
            }
            
            {
                const Event::TextFocus event;
                DJV_ASSERT(event.getEventType() == Event::Type::TextFocus);
            }
            
            {
                const Event::TextFocusLost event;
                DJV_ASSERT(event.getEventType() == Event::Type::TextFocusLost);
            }
            
            {
                std::wstring_convert<std::codecvt_utf8<djv_char_t>, djv_char_t> utf32Convert;
                const std::basic_string<djv_char_t> utf32 = utf32Convert.from_bytes("text");
                const int textModifiers = 1;
                const Event::TextInput event(utf32, textModifiers);
                DJV_ASSERT(event.getEventType() == Event::Type::TextInput);
                DJV_ASSERT(event.getUtf32() == utf32);
                DJV_ASSERT(event.getTextModifiers() == textModifiers);
            }
        }
        
    } // namespace CoreTest
} // namespace djv

