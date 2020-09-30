// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace Observer
        {
            template<typename T>
            inline void ValueObserver<T>::_init(
                const std::weak_ptr<IValueSubject<T> >& value,
                const std::function<void(const T&)>& callback,
                CallbackAction action)
            {
                _subject = value;
                _callback = callback;
                if (auto subject = value.lock())
                {
                    subject->_add(ValueObserver<T>::shared_from_this());
                    if (CallbackAction::Trigger == action)
                    {
                        _callback(subject->get());
                    }
                }
            }

            template<typename T>
            inline ValueObserver<T>::ValueObserver()
            {}

            template<typename T>
            inline ValueObserver<T>::~ValueObserver()
            {
                if (auto subject = _subject.lock())
                {
                    subject->_removeExpired();
                }
            }

            template<typename T>
            inline void ValueObserver<T>::doCallback(const T& value)
            {
                _callback(value);
            }

            template<typename T>
            inline IValueSubject<T>::~IValueSubject()
            {}

            template<typename T>
            inline size_t IValueSubject<T>::getObserversCount() const
            {
                return _observers.size();
            }

            template<typename T>
            inline void IValueSubject<T>::_add(const std::weak_ptr<ValueObserver<T> >& observer)
            {
                _observers.push_back(observer);
            }

            template<typename T>
            inline void IValueSubject<T>::_removeExpired()
            {
                auto i = _observers.begin();
                while (i != _observers.end())
                {
                    if (i->expired())
                    {
                        i = _observers.erase(i);
                    }
                    else
                    {
                        ++i;
                    }
                }
            }

            template<typename T>
            inline std::shared_ptr<Observer::ValueObserver<T> > ValueObserver<T>::create(
                const std::weak_ptr<IValueSubject<T> >& value,
                const std::function<void(const T&)>& callback,
                CallbackAction action)
            {
                std::shared_ptr<Observer::ValueObserver<T> > out(new ValueObserver<T>);
                out->_init(value, callback, action);
                return out;
            }

            template<typename T>
            inline ValueSubject<T>::ValueSubject()
            {}

            template<typename T>
            inline ValueSubject<T>::ValueSubject(const T& value) :
                _value(value)
            {}

            template<typename T>
            inline std::shared_ptr<Observer::ValueSubject<T> > ValueSubject<T>::create()
            {
                return std::shared_ptr<Observer::ValueSubject<T> >(new ValueSubject<T>);
            }

            template<typename T>
            inline std::shared_ptr<Observer::ValueSubject<T> > ValueSubject<T>::create(const T& value)
            {
                return std::shared_ptr<Observer::ValueSubject<T> >(new ValueSubject<T>(value));
            }

            template<typename T>
            inline void ValueSubject<T>::setAlways(const T& value)
            {
                _value = value;
                for (const auto& s : IValueSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline bool ValueSubject<T>::setIfChanged(const T& value)
            {
                if (value == _value)
                    return false;
                _value = value;
                for (const auto& s : IValueSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
                return true;
            }

            template<typename T>
            inline const T& ValueSubject<T>::get() const
            {
                return _value;
            }

        } // namespace Observer
    } // namespace Core
} // namespace djv
