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

//! \file djvFileIoInline.h

//------------------------------------------------------------------------------
// djvFileIo
//------------------------------------------------------------------------------

inline void djvFileIo::get8(qint8 * in, quint64 size) throw (djvError)
{
    return get(in, size, 1);
}

inline void djvFileIo::getU8(quint8 * in, quint64 size) throw (djvError)
{
    return get(in, size, 1);
}

inline void djvFileIo::get16(qint16 * in, quint64 size) throw (djvError)
{
    return get(in, size, 2);
}

inline void djvFileIo::getU16(quint16 * in, quint64 size) throw (djvError)
{
    return get(in, size, 2);
}

inline void djvFileIo::get32(qint32 * in, quint64 size) throw (djvError)
{
    return get(in, size, 4);
}

inline void djvFileIo::getU32(quint32 * in, quint64 size) throw (djvError)
{
    return get(in, size, 4);
}

inline void djvFileIo::getF32(float * in, quint64 size) throw (djvError)
{
    return get(in, size, 4);
}

inline void djvFileIo::set8(const qint8 * in, quint64 size) throw (djvError)
{
    set(in, size, 1);
}

inline void djvFileIo::setU8(const quint8 * in, quint64 size) throw (djvError)
{
    set(in, size, 1);
}

inline void djvFileIo::set16(const qint16 * in, quint64 size) throw (djvError)
{
    set(in, size, 2);
}

inline void djvFileIo::setU16(const quint16 * in, quint64 size) throw (djvError)
{
    set(in, size, 2);
}

inline void djvFileIo::set32(const qint32 * in, quint64 size) throw (djvError)
{
    return set(in, size, 4);
}

inline void djvFileIo::setU32(const quint32 * in, quint64 size) throw (djvError)
{
    return set(in, size, 4);
}

inline void djvFileIo::setF32(const float * in, quint64 size) throw (djvError)
{
    set(in, size, 4);
}

inline void djvFileIo::set8(const qint8 & in) throw (djvError)
{
    set8(&in, 1);
}

inline void djvFileIo::setU8(const quint8 & in) throw (djvError)
{
    setU8(&in, 1);
}

inline void djvFileIo::set16(const qint16 & in) throw (djvError)
{
    set16(&in, 1);
}

inline void djvFileIo::setU16(const quint16 & in) throw (djvError)
{
    setU16(&in, 1);
}

inline void djvFileIo::set32(const qint32 & in) throw (djvError)
{
    set32(&in, 1);
}

inline void djvFileIo::setU32(const quint32 & in) throw (djvError)
{
    setU32(&in, 1);
}

inline void djvFileIo::setF32(const float & in) throw (djvError)
{
    setF32(&in, 1);
}

