//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/Core.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace Core
    {
        template<typename T>
        class IValueSubject;

        //! This class provides a value observer.
        template<typename T>
        class ValueObserver : public std::enable_shared_from_this<ValueObserver<T> >
        {
            DJV_NON_COPYABLE(ValueObserver);
            
        protected:
            inline void _init(
                const std::weak_ptr<IValueSubject<T> >&,
                const std::function<void(const T&)>&);

            inline ValueObserver();

        public:
            inline ~ValueObserver();

            //! Create a new value observer.
            static inline std::shared_ptr<ValueObserver<T> > create(
                const std::weak_ptr<IValueSubject<T> >&,
                const std::function<void(const T&)>&);

            //! Execute the callback.
            inline void doCallback(const T&);

        private:
            std::function<void(const T&)> _callback;
            std::weak_ptr<IValueSubject<T> > _subject;
        };

        //! This class provides an interface for a value subject.
        template<typename T>
        class IValueSubject
        {
        public:
            inline virtual ~IValueSubject() = 0;

            //! Get the value.
            virtual const T& get() const = 0;

        protected:
            inline void _add(const std::weak_ptr<ValueObserver<T> >&);
            inline void _remove(ValueObserver<T>*);

            std::vector<std::weak_ptr<ValueObserver<T> > > _observers;

            friend class ValueObserver<T>;
        };

        //! This class provides a value subject.
        template<typename T>
        class ValueSubject : public IValueSubject<T>
        {
            DJV_NON_COPYABLE(ValueSubject);

        protected:
            inline ValueSubject();
            inline ValueSubject(const T&);

        public:
            //! Create a new value subject.
            static inline std::shared_ptr<ValueSubject<T> > create();

            //! Create a new value subject with the given value.
            static inline std::shared_ptr<ValueSubject<T> > create(const T&);

            //! Set the value.
            inline void setAlways(const T&);

            //! Set the value only if it has changed.
            inline bool setIfChanged(const T&);

            inline const T& get() const override;

        private:
            T _value = T();
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ValueObserverInline.h>

