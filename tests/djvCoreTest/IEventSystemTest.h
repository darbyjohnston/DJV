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
            
            void run(const std::vector<std::string>&) override;
            
        private:
            void _info();
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

