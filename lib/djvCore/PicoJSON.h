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

#pragma once

#include <djvCore/Core.h>

#include <picojson/picojson.h>

namespace djv
{
    namespace Core
    {
        class FileIO;

        namespace PicoJSON
        {
            void write(const picojson::value&, FileIO&, size_t indent = 0, bool continueLine = false);

        } // namespace PicoJSON
    } // namespace Core

    picojson::value toJSON(int);
    picojson::value toJSON(float);
    picojson::value toJSON(const std::string&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, int&);
    void fromJSON(const picojson::value&, float&);
    void fromJSON(const picojson::value&, std::string&);

    //! Convert a value to JSON. This function needs an overload of toString(const T&).
    template<typename T>
    inline picojson::value toJSON(const T&);

    //! Convert a vector of values to JSON. This function needs an overload of toJSON(const T&).
    template<typename T>
    inline picojson::value toJSON(const std::vector<T>&);

    //! Convert a map of values to JSON. This function needs an overload of toString(constT&) and toJSON(const U&).
    template<typename T, typename U>
    inline picojson::value toJSON(const std::map<T, U>&);

    //! Convert JSON to a value. This function needs an overload of fromString(const std::string&, T&);
    //!
    //! Throws:
    //! - Error
    template<typename T>
    inline void fromJSON(const picojson::value&, T&);

    //! Convert JSON to a vector of values. This function needs an overload of fromJSON(const picojson::value&, T&);
    //!
    //! Throws:
    //! - std::exception
    template<typename T>
    inline void fromJSON(const picojson::value&, std::vector<T>&);

    //! Convert JSON to a map of values. This function needs an overload of fromString(const picojson::value&, T&)
    //! and fromJSON(const picojson::value&, U&);
    //!
    //! Throws:
    //! - std::exception
    template<typename T, typename U>
    inline void fromJSON(const picojson::value&, std::map<T, U>&);

} // namespace djv

#include <djvCore/PicoJSONInline.h>
