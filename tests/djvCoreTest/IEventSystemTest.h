// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/TickTest.h>

#include <djvCore/Event.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace CoreTest
    {
        class TestEventSystem;
        class TestObject;
        class TestObject2;
        
        class IEventSystemTest : public Test::ITickTest
        {
        public:
            IEventSystemTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _clipboard();
            void _textFocus();
            void _tick();
            
            std::shared_ptr<TestEventSystem> _system;
            std::shared_ptr<TestObject> _object;
            std::shared_ptr<TestObject2> _object2;
            std::shared_ptr<Core::ValueObserver<Core::Event::PointerInfo> > _pointerObserver;
            std::shared_ptr<Core::ValueObserver<std::shared_ptr<Core::IObject> > > _hoverObserver;
            std::shared_ptr<Core::ValueObserver<std::shared_ptr<Core::IObject> > > _grabObserver;
            std::shared_ptr<Core::ValueObserver<std::shared_ptr<Core::IObject> > > _keyGrabObserver;
        };
        
    } // namespace CoreTest
} // namespace djv

