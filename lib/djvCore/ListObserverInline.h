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

#include <algorithm>

namespace djv
{
    namespace Core
    {
        template<typename T>
        inline void ListObserver<T>::_init(
            const std::weak_ptr<IListSubject<T> > & value,
            const std::function<void(const std::vector<T> &)> & callback)
        {
            _subject = value;
            _callback = callback;
            if (auto subject = value.lock())
            {
                subject->_add(ListObserver<T>::shared_from_this());
                _callback(subject->get());
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
                subject->_remove(this);
            }
        }

        template<typename T>
        inline std::shared_ptr<ListObserver<T> > ListObserver<T>::create(
            const std::weak_ptr<IListSubject<T> > & value,
            const std::function<void(const std::vector<T> &)> & callback)
        {
            std::shared_ptr<ListObserver<T> > out(new ListObserver<T>);
            out->_init(value, callback);
            return out;
        }

        template<typename T>
        inline void ListObserver<T>::doCallback(const std::vector<T> & value)
        {
            _callback(value);
        }

        template<typename T>
        inline IListSubject<T>::~IListSubject()
        {}

        template<typename T>
        inline void IListSubject<T>::_add(const std::weak_ptr<ListObserver<T> > & observer)
        {
            _observers.push_back(observer);
        }

        template<typename T>
        inline void IListSubject<T>::_remove(ListObserver<T>* observer)
        {
            const auto i = std::find_if(
                _observers.begin(),
                _observers.end(),
                [observer](const std::weak_ptr<ListObserver<T> > & other)
            {
                if (auto i = other.lock())
                {
                    return observer == i.get();
                }
                return false;
            });
            if (i != _observers.end())
            {
                _observers.erase(i);
            }
        }

        template<typename T>
        inline ListSubject<T>::ListSubject()
        {}

        template<typename T>
        inline ListSubject<T>::ListSubject(const std::vector<T> & value) :
            _value(value)
        {}

        template<typename T>
        inline std::shared_ptr<ListSubject<T> > ListSubject<T>::create()
        {
            return std::shared_ptr<ListSubject<T> >(new ListSubject<T>);
        }

        template<typename T>
        inline std::shared_ptr<ListSubject<T> > ListSubject<T>::create(const std::vector<T> & value)
        {
            return std::shared_ptr<ListSubject<T> >(new ListSubject<T>(value));
        }

        template<typename T>
        inline void ListSubject<T>::setAlways(const std::vector<T> & value)
        {
            _value = value;
            for (const auto & s : IListSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        inline bool ListSubject<T>::setIfChanged(const std::vector<T> & value)
        {
            if (value == _value)
                return false;
            _value = value;
            for (const auto & s : IListSubject<T>::_observers)
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

                for (const auto & s : IListSubject<T>::_observers)
                {
                    if (auto observer = s.lock())
                    {
                        observer->doCallback(_value);
                    }
                }
            }
        }
        
        template<typename T>
        void ListSubject<T>::setItem(size_t index, const T & value)
        {
            _value[index] = value;
            for (const auto & s : IListSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        void ListSubject<T>::setItemOnlyIfChanged(size_t index, const T & value)
        {
            if (value == _value[index])
                return;
            _value[index] = value;
            for (const auto & s : IListSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        void ListSubject<T>::pushBack(const T & value)
        {
            _value.push_back(value);
            for (const auto & s : IListSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        void ListSubject<T>::removeItem(size_t index)
        {
            _value.erase(_value.begin() + index);
            for (const auto & s : IListSubject<T>::_observers)
            {
                if (auto observer = s.lock())
                {
                    observer->doCallback(_value);
                }
            }
        }

        template<typename T>
        inline const std::vector<T> & ListSubject<T>::get() const
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
        inline const T & ListSubject<T>::getItem(size_t index) const
        {
            return _value[index];
        }

        template<typename T>
        inline bool ListSubject<T>::contains(const T & value) const
        {
            const auto i = std::find_if(
                _value.begin(),
                _value.end(),
                [value](const T & other)
            {
                return value == other;
            });
            return i != _value.end();
        }

        template<typename T>
        inline size_t ListSubject<T>::indexOf(const T & value) const
        {
            const auto i = std::find_if(
                _value.begin(),
                _value.end(),
                [value](const T & other)
            {
                return value == other;
            });
            return i != _value.end() ? i - _value.begin() : static_cast<size_t>(-1);
        }

    } // namespace Core
} // namespace djv
