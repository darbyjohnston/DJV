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

//------------------------------------------------------------------------------
// djvVectorUtil
//------------------------------------------------------------------------------

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

#pragma once

#include <djvCore/Math.h>

//! \addtogroup djvCoreMath
//@{

//------------------------------------------------------------------------------
//! \class djvVectorUtil
//!
//! This class provides vector utilities.
//------------------------------------------------------------------------------

inline djvVectorUtil::~djvVectorUtil()
{}

template<typename T, glm::precision P>
inline bool djvVectorUtil::isSizeValid(const glm::tvec2<T, P> & in)
{
    return in.x > T(0) && in.y > T(0);
}

template<typename T, glm::precision P>
inline glm::tvec2<T, P> djvVectorUtil::min(const glm::tvec2<T, P> & a, const glm::tvec2<T, P> & b)
{
    return glm::tvec2<T, P>(djvMath::min(a.x, b.x), djvMath::min(a.y, b.y));
}

template<typename T, glm::precision P>
inline glm::tvec2<T, P> djvVectorUtil::max(const glm::tvec2<T, P> & a, const glm::tvec2<T, P> & b)
{
    return glm::tvec2<T, P>(djvMath::max(a.x, b.x), djvMath::max(a.y, b.y));
}

template<typename T, glm::precision P>
inline glm::tvec2<T, P> djvVectorUtil::swap(const glm::tvec2<T, P> & in)
{
    return glm::tvec2<T, P>(in.y, in.x);
}

template<typename T, glm::precision P>
inline float djvVectorUtil::aspect(const glm::tvec2<T, P> & in)
{
    return in.y > T(0) ? in.x / static_cast<float>(in.y) : T(0); 
}

inline glm::ivec2 djvVectorUtil::floor(const glm::vec2& value)
{
    return glm::ivec2(djvMath::floor(value.x), djvMath::floor(value.y));
}

inline glm::ivec2 djvVectorUtil::ceil(const glm::vec2& value)
{
    return glm::ivec2(djvMath::ceil(value.x), djvMath::ceil(value.y));
}

