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

namespace djv
{
    namespace UI
    {
        template<typename T>
        inline void ISettings::_read(const std::string& name, const picojson::object& object, T& out)
        {
            const auto i = object.find(name);
            if (i != object.end())
            {
                try
                {
                    fromJSON(i->second, out);
                }
                catch (const std::exception& e)
                {
                    _readError(name, e.what());
                }
            }
        }

        template<typename T>
        inline void ISettings::_read(const std::string& name, const picojson::object& object, std::shared_ptr<Core::ValueSubject<T> >& out)
        {
            const auto i = object.find(name);
            if (i != object.end())
            {
                try
                {
                    T v;
                    fromJSON(i->second, v);
                    out->setIfChanged(v);
                }
                catch (const std::exception& e)
                {
                    _readError(name, e.what());
                }
            }
        }

        template<typename T>
        inline void ISettings::_read(const std::string& name, const picojson::object& object, std::shared_ptr<Core::ListSubject<T> >& out)
        {
            const auto i = object.find(name);
            if (i != object.end())
            {
                try
                {
                    std::vector<T> v;
                    fromJSON(i->second, v);
                    out->setIfChanged(v);
                }
                catch (const std::exception& e)
                {
                    _readError(name, e.what());
                }
            }
        }

        template<typename T, typename U>
        inline void ISettings::_read(const std::string& name, const picojson::object& object, std::shared_ptr<Core::MapSubject<T, U> >& out)
        {
            const auto i = object.find(name);
            if (i != object.end())
            {
                try
                {
                    std::map<T, U> v;
                    fromJSON(i->second, v);
                    out->setIfChanged(v);
                }
                catch (const std::exception& e)
                {
                    _readError(name, e.what());
                }
            }
        }

        template<typename T>
        inline void ISettings::_write(const std::string& name, const T& value, picojson::object& out)
        {
            out[name] = toJSON(value);
        }

    } // namespace UI
} // namespace djv
