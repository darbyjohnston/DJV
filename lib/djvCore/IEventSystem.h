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
            //! This class provides the base functionality for event systems.
            class IEventSystem : public ISystem
            {
                DJV_NON_COPYABLE(IEventSystem);

            protected:
                void _init(const std::string & systemName, const std::shared_ptr<Context>&);
                IEventSystem();

            public:
                ~IEventSystem() override;

                std::shared_ptr<IObject> getRootObject() const;

                std::shared_ptr<Core::IValueSubject<PointerInfo> > observePointer() const;
                std::shared_ptr<Core::IValueSubject<std::shared_ptr<IObject> > > observeHover() const;
                std::shared_ptr<Core::IValueSubject<std::shared_ptr<IObject> > > observeGrab() const;
                std::shared_ptr<Core::IValueSubject<std::shared_ptr<IObject> > > observeKeyGrab() const;

                const std::weak_ptr<IObject> & getTextFocus() const;
                void setTextFocus(const std::shared_ptr<IObject> &);

                virtual void setClipboard(const std::string&);
                virtual std::string getClipboard() const;

                void tick(const std::chrono::steady_clock::time_point&, const Time::Unit&) override;

                //! \todo How can we make this function protected?
                void _objectCreated(const std::shared_ptr<IObject> &);

            protected:
                virtual void _initObject(const std::shared_ptr<IObject> &) {}
                void _initRecursive(const std::shared_ptr<IObject> &, Init &);

                void _pointerMove(const PointerInfo &);
                void _buttonPress(int);
                void _buttonRelease(int);
                void _drop(const std::vector<std::string> &);
                void _keyPress(int key, int modifiers);
                void _keyRelease(int key, int modifiers);
                void _text(const std::basic_string<djv_char_t>& utf32, int modifiers);
                void _scroll(float, float);

                virtual void _hover(PointerMove &, std::shared_ptr<IObject> &) = 0;

            private:
                void _updateRecursive(const std::shared_ptr<IObject> &, Update &);
                void _setHover(const std::shared_ptr<IObject> &);
                void _keyPress(std::shared_ptr<IObject>, KeyPress&);

                DJV_PRIVATE();

                friend class IObject;
            };

        } // namespace Event
    } // namespace Core
} // namespace djv
