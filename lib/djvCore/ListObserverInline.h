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
            inline void ListObserver<T>::_init(
                const std::weak_ptr<IListSubject<T> >& value,
                const std::function<void(const std::vector<T>&)>& callback,
                CallbackAction action)
            {
                _subject = value;
                _callback = callback;
                if (auto subject = value.lock())
                {
                    subject->_add(ListObserver<T>::shared_from_this());
                    if (CallbackAction::Trigger == action)
                    {
                        _callback(subject->get());
                    }
                }
            }

            template<typename T>
            inline ListObserver<T>::ListObserver()
            {}

            template<typename T>
            inline ListObserver<T>::~ListObserver()
            {
                if (auto subject = _subject.lock())
                {
                    subject->_removeExpired();
                }
            }

            template<typename T>
            inline std::shared_ptr<Observer::ListObserver<T> > ListObserver<T>::create(
                const std::weak_ptr<IListSubject<T> >& value,
                const std::function<void(const std::vector<T>&)>& callback,
                CallbackAction action)
            {
                std::shared_ptr<Observer::ListObserver<T> > out(new ListObserver<T>);
                out->_init(value, callback, action);
                return out;
            }

            template<typename T>
            inline void ListObserver<T>::doCallback(const std::vector<T>& value)
            {
                _callback(value);
            }

            template<typename T>
            inline IListSubject<T>::~IListSubject()
            {}

            template<typename T>
            inline size_t IListSubject<T>::getObserversCount() const
            {
                return _observers.size();
            }

            template<typename T>
            inline void IListSubject<T>::_add(const std::weak_ptr<ListObserver<T> >& observer)
            {
                _observers.push_back(observer);
            }

            template<typename T>
            inline void IListSubject<T>::_removeExpired()
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
            inline ListSubject<T>::ListSubject()
            {}

            template<typename T>
            inline ListSubject<T>::ListSubject(const std::vector<T>& value) :
                _value(value)
            {}

            template<typename T>
            inline std::shared_ptr<Observer::ListSubject<T> > ListSubject<T>::create()
            {
                return std::shared_ptr<Observer::ListSubject<T> >(new ListSubject<T>);
            }

            template<typename T>
            inline std::shared_ptr<Observer::ListSubject<T> > ListSubject<T>::create(const std::vector<T>& value)
            {
                return std::shared_ptr<Observer::ListSubject<T> >(new ListSubject<T>(value));
            }

            template<typename T>
            inline void ListSubject<T>::setAlways(const std::vector<T>& value)
            {
                _value = value;
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline bool ListSubject<T>::setIfChanged(const std::vector<T>& value)
            {
                if (value == _value)
                    return false;
                _value = value;
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
                return true;
            }

            template<typename T>
            inline void ListSubject<T>::clear()
            {
                if (_value.size())
                {
                    _value.clear();
                    for (const auto& s : IListSubject<T>::_observers)
                    {
                        if (auto observer = s.lock())
                        {
                            observer->doCallback(_value);
                        }
                    }
                }
            }

            template<typename T>
            inline void ListSubject<T>::setItem(size_t index, const T& value)
            {
                _value[index] = value;
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline void ListSubject<T>::setItemOnlyIfChanged(size_t index, const T& value)
            {
                if (value == _value[index])
                    return;
                _value[index] = value;
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline void ListSubject<T>::pushBack(const T& value)
            {
                _value.push_back(value);
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline void ListSubject<T>::removeItem(size_t index)
            {
                _value.erase(_value.begin() + index);
                for (const auto& s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }

            template<typename T>
            inline const std::vector<T>& ListSubject<T>::get() const
            {
                return _value;
            }

            template<typename T>
            inline size_t ListSubject<T>::getSize() const
            {
                return _value.size();
            }

            template<typename T>
            inline bool ListSubject<T>::isEmpty() const
            {
                return _value.empty();
            }

            template<typename T>
            inline const T& ListSubject<T>::getItem(size_t index) const
            {
                return _value[index];
            }

            template<typename T>
            inline bool ListSubject<T>::contains(const T& value) const
            {
                const auto i = std::find_if(
                    _value.begin(),
                    _value.end(),
                    [value](const T& other)
                    {
                        return value == other;
                    });
                return i != _value.end();
            }

            template<typename T>
            inline size_t ListSubject<T>::indexOf(const T& value) const
            {
                const auto i = std::find_if(
                    _value.begin(),
                    _value.end(),
                    [value](const T& other)
                    {
                        return value == other;
                    });
                return i != _value.end() ? i - _value.begin() : invalidListIndex;
            }

        } // namespace Observer
    } // namespace Core
} // namespace djv
