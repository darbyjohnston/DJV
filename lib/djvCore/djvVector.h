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

#include <djvDebug.h>

#include <QMetaType>
#include <QStringList>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

//! \addtogroup djvCoreMath
//@{

template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator >> (QStringList &, glm::tvec2<T, P> &) throw (QString);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator >> (QStringList &, glm::tvec3<T, P> &) throw (QString);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator >> (QStringList &, glm::tvec4<T, P> &) throw (QString);

template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator << (QStringList &, const glm::tvec2<T, P> &);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator << (QStringList &, const glm::tvec3<T, P> &);
template<typename T, glm::precision P = glm::defaultp>
inline QStringList & operator << (QStringList &, const glm::tvec4<T, P> &);

template <typename T, glm::precision P = glm::defaultp>
inline djvDebug & operator << (djvDebug &, const glm::tvec2<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvDebug & operator << (djvDebug &, const glm::tvec3<T, P> &);
template <typename T, glm::precision P = glm::defaultp>
inline djvDebug & operator << (djvDebug &, const glm::tvec4<T, P> &);

Q_DECLARE_METATYPE(glm::ivec2)
Q_DECLARE_METATYPE(glm::ivec3)
Q_DECLARE_METATYPE(glm::ivec4)
Q_DECLARE_METATYPE(glm::vec2)
Q_DECLARE_METATYPE(glm::vec3)
Q_DECLARE_METATYPE(glm::vec4)

//@} // djvCoreMath

#include <djvVectorInline.h>

