// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

template<typename T>
inline IOQueue<T>::IOQueue(size_t max) :
    _max(max)
{}

template<typename T>
inline size_t IOQueue<T>::getMax() const
{
    return _max;
}

template<typename T>
inline void IOQueue<T>::setMax(size_t value)
{
    _max = value;
}

template<typename T>
inline bool IOQueue<T>::isEmpty() const
{
    return 0 == _queue.size();
}

template<typename T>
inline size_t IOQueue<T>::getCount() const
{
    return _queue.size();
}

template<typename T>
inline T IOQueue<T>::getFrame() const
{
    return _queue.size() ? _queue.front() : T();
}

template<typename T>
inline void IOQueue<T>::addFrame(const T& value)
{
    _queue.push(value);
}

template<typename T>
inline T IOQueue<T>::popFrame()
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
inline void IOQueue<T>::clearFrames()
{
    while (_queue.size())
    {
        _queue.pop();
    }
}

template<typename T>
inline bool IOQueue<T>::isFinished() const
{
    return _finished;
}

template<typename T>
inline void IOQueue<T>::setFinished(bool value)
{
    _finished = value;
}
