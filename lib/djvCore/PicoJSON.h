// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <picojson/picojson.h>

#include <memory>
#include <string>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileIO;

        } // namespace FileSystem

        //! This namespace provides JSON functionality.
        namespace PicoJSON
        {
            void write(
                const picojson::value&,
                const std::shared_ptr<FileSystem::FileIO>&,
                size_t indent = 0,
                bool continueLine = false);

        } // namespace PicoJSON
    } // namespace Core

    picojson::value toJSON(bool);
    picojson::value toJSON(int);
    picojson::value toJSON(float);
    picojson::value toJSON(size_t);
    picojson::value toJSON(const std::string&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, bool&);
    void fromJSON(const picojson::value&, int&);
    void fromJSON(const picojson::value&, float&);
    void fromJSON(const picojson::value&, size_t&);
    void fromJSON(const picojson::value&, std::string&);

} // namespace djv
