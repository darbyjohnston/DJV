// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/RapidJSON.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            //! Throws:
            //! - std::exception
            template<typename T>
            void read(const std::string & name, const rapidjson::Value&, T&);
            template<typename T>
            void read(const std::string & name, const rapidjson::Value&, std::shared_ptr<Core::ValueSubject<T> >&);
            template<typename T>
            void read(const std::string & name, const rapidjson::Value&, std::shared_ptr<Core::ListSubject<T> >&);
            template<typename T>
            void read(const std::string & name, const rapidjson::Value&, std::shared_ptr<Core::MapSubject<std::string, T> >&);

            template<typename T>
            void write(const std::string & name, const T &, rapidjson::Value&, rapidjson::Document::AllocatorType&);

        } // namespace Settings
    } // namespace UI
} // namespace djv

#include <djvUI/ISettingsTemplatesInline.h>
