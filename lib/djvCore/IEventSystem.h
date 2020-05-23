// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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

                void tick() override;

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
