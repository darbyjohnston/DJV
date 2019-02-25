//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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
        inline void ValueObserver<T>::_init(const std::weak_ptr<IValueSubject<T> >& value, const std::function<void(const T&)>& callback)
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
        inline void ValueObserver<T>::doCallback(const T& value)
        {
            _callback(value);
        }

        template<typename T>
        inline IValueSubject<T>::~IValueSubject()
        {}

        template<typename T>
        inline void IValueSubject<T>::_add(const std::weak_ptr<ValueObserver<T> >& observer)
        {
            _observers.push_back(observer);
        }

        template<typename T>
        inline void IValueSubject<T>::_remove(ValueObserver<T>* observer)
        {
            const auto i = std::find_if(
                _observers.begin(),
                _observers.end(),
                [observer](const std::weak_ptr<ValueObserver<T> >& other)
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
        inline std::shared_ptr<ValueObserver<T> > ValueObserver<T>::create(const std::weak_ptr<IValueSubject<T> >& value, const std::function<void(const T&)>& callback)
        {
            std::shared_ptr<ValueObserver<T> > out(new ValueObserver<T>);
            out->_init(value, callback);
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
        inline std::shared_ptr<ValueSubject<T> > ValueSubject<T>::create()
        {
            return std::shared_ptr<ValueSubject<T> >(new ValueSubject<T>);
        }

        template<typename T>
        inline std::shared_ptr<ValueSubject<T> > ValueSubject<T>::create(const T& value)
        {
            return std::shared_ptr<ValueSubject<T> >(new ValueSubject<T>(value));
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

    } // namespace Core
} // namespace djv
