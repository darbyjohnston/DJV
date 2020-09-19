// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Core
    {
        template<typename T, typename U>
        inline void MapObserver<T, U>::_init(
            const std::weak_ptr<IMapSubject<T, U> >& value,
            const std::function<void(const std::map<T, U>&)>& callback)
        {
            _subject = value;
            _callback = callback;
            if (auto subject = value.lock())
            {
                subject->_add(MapObserver<T, U>::shared_from_this());
                _callback(subject->get());
            }
        }

        template<typename T, typename U>
        inline MapObserver<T, U>::MapObserver()
        {}

        template<typename T, typename U>
        inline MapObserver<T, U>::~MapObserver()
        {
            if (auto subject = _subject.lock())
            {
                subject->_removeExpired();
            }
        }

        template<typename T, typename U>
        inline std::shared_ptr<MapObserver<T, U> > MapObserver<T, U>::create(
            const std::weak_ptr<IMapSubject<T, U> >& value,
            const std::function<void(const std::map<T, U>&)>& callback)
        {
            std::shared_ptr<MapObserver<T, U> > out(new MapObserver<T, U>);
            out->_init(value, callback);
            return out;
        }

        template<typename T, typename U>
        inline void MapObserver<T, U>::doCallback(const std::map<T, U>& value)
        {
            _callback(value);
        }

        template<typename T, typename U>
        inline IMapSubject<T, U>::~IMapSubject()
        {}
        
        template<typename T, typename U>
        inline size_t IMapSubject<T, U>::getObserversCount() const
        {
            return _observers.size();
        }

        template<typename T, typename U>
        inline void IMapSubject<T, U>::_add(const std::weak_ptr<MapObserver<T, U> >& observer)
        {
            _observers.push_back(observer);
        }

        template<typename T, typename U>
        inline void IMapSubject<T, U>::_removeExpired()
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

        template<typename T, typename U>
        inline MapSubject<T, U>::MapSubject()
        {}

        template<typename T, typename U>
        inline MapSubject<T, U>::MapSubject(const std::map<T, U>& value) :
            _value(value)
        {}

        template<typename T, typename U>
        inline std::shared_ptr<MapSubject<T, U> > MapSubject<T, U>::create()
        {
            return std::shared_ptr<MapSubject<T, U> >(new MapSubject<T, U>);
        }

        template<typename T, typename U>
        inline std::shared_ptr<MapSubject<T, U> > MapSubject<T, U>::create(const std::map<T, U>& value)
        {
            return std::shared_ptr<MapSubject<T, U> >(new MapSubject<T, U>(value));
        }

        template<typename T, typename U>
        inline void MapSubject<T, U>::setAlways(const std::map<T, U>& value)
        {
            _value = value;
            for (const auto& s : IMapSubject<T, U>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T, typename U>
        inline bool MapSubject<T, U>::setIfChanged(const std::map<T, U>& value)
        {
            if (value == _value)
                return false;
            _value = value;
            for (const auto& s : IMapSubject<T, U>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
            return true;
        }

        template<typename T, typename U>
        inline void MapSubject<T, U>::clear()
        {
            if (_value.size())
            {
                _value.clear();
                for (const auto& s : IMapSubject<T, U>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }
        }

        template<typename T, typename U>
        inline void MapSubject<T, U>::setItem(const T& key, const U& value)
        {
            _value[key] = value;

            for (const auto& s : IMapSubject<T, U>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T, typename U>
        inline void MapSubject<T, U>::setItemOnlyIfChanged(const T& key, const U& value)
        {
            const auto i = _value.find(key);
            if (i != _value.end() && i->second == value)
                return;
            _value[key] = value;
            for (const auto& s : IMapSubject<T, U>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T, typename U>
        inline const std::map<T, U>& MapSubject<T, U>::get() const
        {
            return _value;
        }

        template<typename T, typename U>
        inline size_t MapSubject<T, U>::getSize() const
        {
            return _value.size();
        }

        template<typename T, typename U>
        inline bool MapSubject<T, U>::isEmpty() const
        {
            return _value.empty();
        }

        template<typename T, typename U>
        inline bool MapSubject<T, U>::hasKey(const T& value)
        {
            return _value.find(value) != _value.end();
        }

        template<typename T, typename U>
        inline const U& MapSubject<T, U>::getItem(const T& key) const
        {
            return _value.find(key)->second;
        }

    } // namespace Core
} // namespace djv
