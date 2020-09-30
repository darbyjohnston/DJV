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
            inline void read(const std::string& name, const rapidjson::Value& object, T& out)
            {
                const auto i = object.FindMember(name.c_str());
                if (i != object.MemberEnd())
                {
                    try
                    {
                        fromJSON(i->value, out);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "': " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string& name, const rapidjson::Value& object, std::shared_ptr<Core::Observer::ValueSubject<T> >& out)
            {
                const auto i = object.FindMember(name.c_str());
                if (i != object.MemberEnd())
                {
                    try
                    {
                        T v;
                        fromJSON(i->value, v);
                        out->setIfChanged(v);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "': " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string& name, const rapidjson::Value& object, std::shared_ptr<Core::Observer::ListSubject<T> >& out)
            {
                const auto i = object.FindMember(name.c_str());
                if (i != object.MemberEnd())
                {
                    try
                    {
                        std::vector<T> v;
                        fromJSON(i->value, v);
                        out->setIfChanged(v);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "': " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void read(const std::string& name, const rapidjson::Value& object, std::shared_ptr<Core::Observer::MapSubject<std::string, T> >& out)
            {
                const auto i = object.FindMember(name.c_str());
                if (i != object.MemberEnd())
                {
                    try
                    {
                        std::map<std::string, T> v;
                        fromJSON(i->value, v);
                        out->setIfChanged(v);
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings '" << name << "': " << e.what();
                        throw std::invalid_argument(ss.str());
                    }
                }
            }

            template<typename T>
            inline void write(const std::string& name, const T& value, rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator)
            {
                out.AddMember(rapidjson::Value(name.c_str(), allocator), toJSON(value, allocator), allocator);
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv
