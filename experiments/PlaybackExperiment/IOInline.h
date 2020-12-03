// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace IO
{
    template<typename T>
    inline Queue<T>::Queue(size_t max) :
        _max(max)
    {}

    template<typename T>
    inline size_t Queue<T>::getMax() const
    {
        return _max;
    }

    template<typename T>
    inline void Queue<T>::setMax(size_t value)
    {
        _max = value;
    }

    template<typename T>
    inline bool Queue<T>::isEmpty() const
    {
        return 0 == _queue.size();
    }

    template<typename T>
    inline size_t Queue<T>::getCount() const
    {
        return _queue.size();
    }

    template<typename T>
    inline T Queue<T>::getFrame() const
    {
        return _queue.size() ? _queue.front() : T();
    }

    template<typename T>
    inline void Queue<T>::addFrame(const T& value)
    {
        _queue.push(value);
    }

    template<typename T>
    inline T Queue<T>::popFrame()
    {
        T out;
        if (_queue.size())
        {
            out = _queue.front();
            _queue.pop();
        }
        return out;
    }

    template<typename T>
    inline void Queue<T>::clearFrames()
    {
        while (_queue.size())
        {
            _queue.pop();
        }
    }

    template<typename T>
    inline bool Queue<T>::isFinished() const
    {
        return _finished;
    }

    template<typename T>
    inline void Queue<T>::setFinished(bool value)
    {
        _finished = value;
    }

} // namespace IO
