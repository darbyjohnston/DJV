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

#pragma once

#include <djvCore/Event.h>
#include <djvCore/ISystem.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class IObject;

        namespace Event
        {
            class IEventSystem : public ISystem
            {
                DJV_NON_COPYABLE(IEventSystem);

            protected:
                void _init(const std::string & systemName, Context *);
                IEventSystem();

            public:
                virtual ~IEventSystem() = 0;

                std::shared_ptr<IObject> getRootObject() const;

                std::shared_ptr<Core::IValueSubject<PointerInfo> > observePointer() const;

                const std::weak_ptr<IObject> & getTextFocus() const;
                void setTextFocus(const std::shared_ptr<IObject> &);

                void tick(float dt) override;

                void _objectCreated(const std::shared_ptr<IObject> &);

            protected:
                virtual void _initObject(const std::shared_ptr<IObject> &);

                void _pointerMove(const Event::PointerInfo &);
                void _buttonPress(int);
                void _buttonRelease(int);
                void _drop(const std::vector<std::string> &);
                void _keyPress(int key, int modifiers);
                void _keyRelease(int key, int modifiers);
                void _text(const std::string &, int modifiers);

                virtual void _hover(Event::PointerMove &, std::shared_ptr<IObject> &) = 0;

            private:
                void _localeRecursive(const std::shared_ptr<IObject> &, Locale &);
                void _updateRecursive(const std::shared_ptr<IObject> &, Update &);
                void _setHover(const std::shared_ptr<IObject> &);

                DJV_PRIVATE();

                friend class IObject;
            };

        } // namespace Event
    } // namespace Core
} // namespace djv
