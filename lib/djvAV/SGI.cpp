// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/SGI.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace SGI
            {
                Plugin::Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("plugin_sgi_io"),
                        fileExtensions,
                        context);
                    return out;
                }

                std::shared_ptr<IRead> Plugin::read(const System::File::Info& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
                }

            } // namespace SGI
        } // namespace IO
    } // namespace AV
} // namespace djv

