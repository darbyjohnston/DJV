// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2008-2009 Mikael Sundell
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IFF.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IFF
        {
            Plugin::Plugin()
            {}

            std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Plugin>(new Plugin);
                out->_init(
                    pluginName,
                    DJV_TEXT("plugin_iff_io"),
                    fileExtensions,
                    context);
                return out;
            }

            std::shared_ptr<IO::IRead> Plugin::read(const System::File::Info& fileInfo, const IO::ReadOptions& options) const
            {
                return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
            }

        } // namespace IFF
    } // namespace AV
} // namespace djv
