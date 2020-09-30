// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/Event.h>
#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace System
    {
        class IObject;

        namespace Event
        {
            //! This class provides the base functionality for event systems.
            class IEventSystem : public ISystem
            {
                DJV_NON_COPYABLE(IEventSystem);

            protected:
                void _init(const std::string& systemName, const std::shared_ptr<Context>&);
                IEventSystem();

            public:
                ~IEventSystem() override;

                //! \name Pointer and Key Interaction
                ///@{

                std::shared_ptr<Core::Observer::IValueSubject<PointerInfo> > observePointer() const;
                std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<IObject> > > observeHover() const;
                std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<IObject> > > observeGrab() const;
                std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<IObject> > > observeKeyGrab() const;

                ///@}

                //! \name Text Focus
                ///@{

                const std::weak_ptr<IObject>& getTextFocus() const;
                
                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeTextFocusActive() const;
                
                void setTextFocus(const std::shared_ptr<IObject>&);

                ///@}

                //! \name Clipboard
                ///@{

                virtual std::string getClipboard() const;
                
                virtual void setClipboard(const std::string&);

                ///@}

                void tick() override;

            protected:
                virtual void _init(Init&) = 0;
                void _initRecursive(const std::shared_ptr<IObject>&, Init&);
                virtual void _update(Update&) = 0;
                void _updateRecursive(const std::shared_ptr<IObject>&, Update&);

                void _pointerMove(const PointerInfo&);
                void _buttonPress(int);
                void _buttonRelease(int);
                void _drop(const std::vector<std::string>&);
                void _keyPress(int key, int modifiers);
                void _keyRelease(int key, int modifiers);
                void _text(const std::basic_string<djv_char_t>& utf32, int modifiers);
                void _scroll(float, float);

                virtual void _hover(PointerMove&, std::shared_ptr<IObject>&) = 0;

            private:
                void _setHover(const std::shared_ptr<IObject>&);
                void _keyPress(std::shared_ptr<IObject>, KeyPress&);

                DJV_PRIVATE();
            };

        } // namespace Event
    } // namespace System
} // namespace djv
