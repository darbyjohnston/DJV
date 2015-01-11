//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvMemoryBufferInline.h

#include <djvMemory.h>

//------------------------------------------------------------------------------
// djvMemoryBuffer
//------------------------------------------------------------------------------

template<typename T>
inline djvMemoryBuffer<T>::djvMemoryBuffer() :
    _data(0),
    _size(0)
{}

template<typename T>
inline djvMemoryBuffer<T>::djvMemoryBuffer(const djvMemoryBuffer & in) :
    _data(0),
    _size(0)
{
    setSize(in._size);
    
    djvMemory::copy(in._data, _data, _size);
}

template<typename T>
inline djvMemoryBuffer<T>::djvMemoryBuffer(quint64 size) :
    _data(0),
    _size(0)
{
    setSize(size);
}

template<typename T>
inline djvMemoryBuffer<T>::~djvMemoryBuffer()
{
    del();
}

template<typename T>
inline quint64 djvMemoryBuffer<T>::size() const
{
    return _size;
}

template<typename T>
inline void djvMemoryBuffer<T>::setSize(quint64 size, bool zero)
{
    if (size == _size)
        return;

    del();

    _size = size;

    _data = new T [_size];

    if (zero)
    {
        this->zero();
    }
}

template<typename T>
inline const T * djvMemoryBuffer<T>::data() const
{
    return _data;
}

template<typename T>
inline const T * djvMemoryBuffer<T>::operator () () const
{
    return _data;
}

template<typename T>
inline T * djvMemoryBuffer<T>::data()
{
    return _data;
}

template<typename T>
inline T * djvMemoryBuffer<T>::operator () ()
{
    return _data;
}

template<typename T>
inline void djvMemoryBuffer<T>::zero()
{
    djvMemory::zero(_data, _size * sizeof(T));
}

template<typename T>
inline djvMemoryBuffer<T> & djvMemoryBuffer<T>::operator = (
    const djvMemoryBuffer<T> & buffer)
{
    if (&buffer != this)
    {
        setSize(buffer._size);
        
        djvMemory::copy(buffer._data, _data, _size);
    }

    return *this;
}

template<typename T>
inline void djvMemoryBuffer<T>::del()
{
    if (_data)
    {
        delete [] _data;
        
        _data = 0;
        _size = 0;
    }
}

