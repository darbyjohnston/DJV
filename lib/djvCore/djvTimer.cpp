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

//! \file djvTimer.cpp

#include <djvTimer.h>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <float.h>
#include <time.h>

//------------------------------------------------------------------------------
// djvTimer::P
//------------------------------------------------------------------------------

struct djvTimer::P
{
#if defined(DJV_WINDOWS)
    //DWORD t0;
    //DWORD t1;
    LARGE_INTEGER t0;
    LARGE_INTEGER t1;
    LARGE_INTEGER frequency;
#else
    ::timeval     t0;
    ::timeval     t1;
#endif
};

//------------------------------------------------------------------------------
// Timer
//------------------------------------------------------------------------------

djvTimer::djvTimer() :
    _p(new P)
{
#if defined(DJV_WINDOWS)

    //_p->t0 = _p->t1 = 0;
    
    _p->t0.QuadPart = 0;
    _p->t1.QuadPart = 0;
    ::QueryPerformanceFrequency(&_p->frequency);

#else // DJV_WINDOWS

    _p->t0.tv_sec = _p->t0.tv_usec = 0;
    _p->t1.tv_sec = _p->t1.tv_usec = 0;

#endif // DJV_WINDOWS

    start();
}

djvTimer::djvTimer(const djvTimer & timer) :
    _p(new P)
{
#if defined(DJV_WINDOWS)

    _p->t0 = timer._p->t0;
    _p->t1 = timer._p->t1;
    _p->frequency = timer._p->frequency;

#else // DJV_WINDOWS

    _p->t0 = timer._p->t0;
    _p->t1 = timer._p->t1;

#endif // DJV_WINDOWS
}

djvTimer::~djvTimer()
{
    delete _p;
}

void djvTimer::start()
{
#if defined(DJV_WINDOWS)

    //_p->t0 = ::GetTickCount();
    
    ::QueryPerformanceCounter(&_p->t0);

#else // DJV_WINDOWS

    ::gettimeofday(&_p->t0, 0);

#endif // DJV_WINDOWS

    _p->t1 = _p->t0;
}

void djvTimer::check()
{
#if defined(DJV_WINDOWS)

    //_p->t1 = ::GetTickCount();
    
    ::QueryPerformanceCounter(&_p->t1);

#else // DJV_WINDOWS

    ::gettimeofday(&_p->t1, 0);

#endif // DJV_WINDOWS
}

#if ! defined(DJV_WINDOWS)

namespace
{

inline void timediff(const timeval & a, const timeval & b, timeval & out)
{
    out.tv_sec  = a.tv_sec  - b.tv_sec;
    out.tv_usec = a.tv_usec - b.tv_usec;

    if (out.tv_usec < 0)
    {
        --out.tv_sec;
        
        out.tv_usec += 1000000;
    }
}

} // namespace

#endif // ! DJV_WINDOWS

double djvTimer::seconds() const
{
    double out = 0.0;

#if defined(DJV_WINDOWS)

    //out = (_p->t1 - _p->t0) / 1000.0;
    
    if (_p->frequency.QuadPart)
    {
        out = (_p->t1.QuadPart - _p->t0.QuadPart) /
            static_cast<double>(_p->frequency.QuadPart);
    }

#else // DJV_WINDOWS

    timeval t;
    timediff(_p->t1, _p->t0, t);
    out = t.tv_sec + t.tv_usec / 1000000.0;

#endif // DJV_WINDOWS

    return out;
}

double djvTimer::fps() const
{
    const double seconds = this->seconds();
    
    return (seconds != 0.0) ? (1.0 / seconds) : 0.0;
}

djvTimer & djvTimer::operator = (const djvTimer & timer)
{
    if (&timer != this)
    {
#if defined(DJV_WINDOWS)

        _p->t0 = timer._p->t0;
        _p->t1 = timer._p->t1;
        _p->frequency = timer._p->frequency;

#else // DJV_WINDOWS

        _p->t0 = timer._p->t0;
        _p->t1 = timer._p->t1;

#endif // DJV_WINDOWS
    }
    
    return *this;
}

