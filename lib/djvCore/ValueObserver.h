// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Observer.h>

#include <functional>
#include <memory>
#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Observer
        {
            template<typename T>
            class IValueSubject;

            //! This class provides a value observer.
            template<typename T>
            class Value : public std::enable_shared_from_this<Value<T> >
            {
                DJV_NON_COPYABLE(Value);

            protected:
                void _init(
                    const std::weak_ptr<IValueSubject<T> >&,
                    const std::function<void(const T&)>&,
                    CallbackAction);

                Value();

            public:
                ~Value();

                //! Create a new value observer.
                static std::shared_ptr<Value<T> > create(
                    const std::weak_ptr<IValueSubject<T> >&,
                    const std::function<void(const T&)>&,
                    CallbackAction = CallbackAction::Trigger);

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
                void _add(const std::weak_ptr<Value<T> >&);
                void _removeExpired();

                std::vector<std::weak_ptr<Value<T> > > _observers;

                friend class Value<T>;
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

        } // namespace Observer
    } // namespace Core
} // namespace djv

#include <djvCore/ValueObserverInline.h>

