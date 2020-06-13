// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/PicoJSON.h>

namespace djv
{
    template<typename T>
    picojson::value toJSON(const std::vector<T>&);
    template<typename T>
    picojson::value toJSON(const std::map<std::string, T>&);

    //! Throws:
    //! - std::exception
    template<typename T>
    void fromJSON(const picojson::value&, std::vector<T>&);
    template<typename T>
    void fromJSON(const picojson::value&, std::map<std::string, T>&);

} // namespace djv

#include <djvCore/PicoJSONTemplatesInline.h>
