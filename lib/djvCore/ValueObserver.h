// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <functional>
#include <memory>
#include <vector>

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
            void _init(
                const std::weak_ptr<IValueSubject<T> >&,
                const std::function<void(const T&)>&);

            ValueObserver();

        public:
            ~ValueObserver();

            //! Create a new value observer.
            static std::shared_ptr<ValueObserver<T> > create(
                const std::weak_ptr<IValueSubject<T> >&,
                const std::function<void(const T&)>&);

            //! Execute the callback.
            void doCallback(const T&);

        private:
            std::function<void(const T&)> _callback;
            std::weak_ptr<IValueSubject<T> > _subject;
        };

        //! This class provides an interface for a value subject.
        template<typename T>
        class IValueSubject
        {
        public:
            virtual ~IValueSubject() = 0;

            //! Get the value.
            virtual const T& get() const = 0;
            
            //! Get the number of observers.
            size_t getObserversCount() const;

        protected:
            void _add(const std::weak_ptr<ValueObserver<T> >&);
            void _remove(ValueObserver<T> *);

            std::vector<std::weak_ptr<ValueObserver<T> > > _observers;

            friend class ValueObserver<T>;
        };

        //! This class provides a value subject.
        template<typename T>
        class ValueSubject : public IValueSubject<T>
        {
            DJV_NON_COPYABLE(ValueSubject);

        protected:
            ValueSubject();
            explicit ValueSubject(const T&);

        public:
            //! Create a new value subject.
            static std::shared_ptr<ValueSubject<T> > create();

            //! Create a new value subject with the given value.
            static std::shared_ptr<ValueSubject<T> > create(const T&);

            //! Set the value.
            void setAlways(const T&);

            //! Set the value only if it has changed.
            bool setIfChanged(const T&);

            const T& get() const override;

        private:
            T _value = T();
        };

    } // namespace Core
} // namespace djv

#include <djvCore/ValueObserverInline.h>

