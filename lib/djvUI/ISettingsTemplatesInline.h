// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <sstream>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            template<typename T>
            inline void read(const std::string & name, const picojson::object & object, T & out)
            {
                const auto i = object.find(name);
                if (i != object.end())
                {
                    try
                    {
                        fromJSON(i->second, out);
                    }
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "'. " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string & name, const picojson::object & object, std::shared_ptr<Core::ValueSubject<T> > & out)
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
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "'. " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string & name, const picojson::object & object, std::shared_ptr<Core::ListSubject<T> > & out)
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
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "'. " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string & name, const picojson::object & object, std::shared_ptr<Core::MapSubject<std::string, T> > & out)
            {
                const auto i = object.find(name);
                if (i != object.end())
                {
                    try
                    {
                        std::map<std::string, T> v;
                        fromJSON(i->second, v);
                        out->setIfChanged(v);
                    }
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "'. " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void write(const std::string & name, const T & value, picojson::object & out)
            {
                out[name] = toJSON(value);
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv
