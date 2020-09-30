// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/TickTest.h>

#include <djvSystem/Event.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace SystemTest
    {
        class TestEventSystem;
        class TestObject;
        class TestObject2;
        
        class IEventSystemTest : public Test::ITickTest
        {
        public:
            IEventSystemTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _clipboard();
            void _textFocus();
            void _tick();
            
            std::shared_ptr<TestEventSystem> _system;
            std::shared_ptr<TestObject> _object;
            std::shared_ptr<TestObject2> _object2;
            std::shared_ptr<Core::Observer::ValueObserver<System::Event::PointerInfo> > _pointerObserver;
            std::shared_ptr<Core::Observer::ValueObserver<std::shared_ptr<System::IObject> > > _hoverObserver;
            std::shared_ptr<Core::Observer::ValueObserver<std::shared_ptr<System::IObject> > > _grabObserver;
            std::shared_ptr<Core::Observer::ValueObserver<std::shared_ptr<System::IObject> > > _keyGrabObserver;
            std::shared_ptr<Core::Observer::ValueObserver<bool> > _textFocusActiveObserver;
        };
        
    } // namespace SystemTest
} // namespace djv

