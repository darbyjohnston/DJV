// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Core
    {
        template<typename T>
        inline void ValueObserver<T>::_init(const std::weak_ptr<IValueSubject<T> > & value, const std::function<void(const T &)> & callback)
        {
            _subject = value;
            _callback = callback;
            if (auto subject = value.lock())
            {
                subject->_add(ValueObserver<T>::shared_from_this());
                _callback(subject->get());
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
                subject->_remove(this);
            }
        }

        template<typename T>
        inline void ValueObserver<T>::doCallback(const T & value)
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
        inline void IValueSubject<T>::_add(const std::weak_ptr<ValueObserver<T> > & observer)
        {
            _observers.push_back(observer);
        }

        template<typename T>
        inline void IValueSubject<T>::_remove(ValueObserver<T> * observer)
        {
            auto i = _observers.begin();
            while (i != _observers.end())
            {
                bool erase = false;
                if (auto j = i->lock())
                {
                    if (observer == j.get())
                    {
                        erase = true;
                    }
                }
                else
                {
                    erase = true;
                }
                if (erase)
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
        inline std::shared_ptr<ValueObserver<T> > ValueObserver<T>::create(const std::weak_ptr<IValueSubject<T> > & value, const std::function<void(const T &)> & callback)
        {
            std::shared_ptr<ValueObserver<T> > out(new ValueObserver<T>);
            out->_init(value, callback);
            return out;
        }

        template<typename T>
        inline ValueSubject<T>::ValueSubject()
        {}

        template<typename T>
        inline ValueSubject<T>::ValueSubject(const T & value) :
            _value(value)
        {}

        template<typename T>
        inline std::shared_ptr<ValueSubject<T> > ValueSubject<T>::create()
        {
            return std::shared_ptr<ValueSubject<T> >(new ValueSubject<T>);
        }

        template<typename T>
        inline std::shared_ptr<ValueSubject<T> > ValueSubject<T>::create(const T & value)
        {
            return std::shared_ptr<ValueSubject<T> >(new ValueSubject<T>(value));
        }

        template<typename T>
        inline void ValueSubject<T>::setAlways(const T & value)
        {
            _value = value;
            for (const auto & s : IValueSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        inline bool ValueSubject<T>::setIfChanged(const T & value)
        {
            if (value == _value)
                return false;
            _value = value;
            for (const auto & s : IValueSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
            return true;
        }

        template<typename T>
        inline const T & ValueSubject<T>::get() const
        {
            return _value;
        }

    } // namespace Core
} // namespace djv
