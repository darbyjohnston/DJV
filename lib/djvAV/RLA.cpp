// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/RLA.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace RLA
        {
            struct Plugin::Private
            {};

            Plugin::Plugin() :
                _p(new Private)
            {}

            std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Plugin>(new Plugin);
                out->_init(
                    pluginName,
                    DJV_TEXT("plugin_rla_io"),
                    fileExtensions,
                    context);
                return out;
            }

            std::shared_ptr<IO::IRead> Plugin::read(const System::File::Info& fileInfo, const IO::ReadOptions& options) const
            {
                return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
            }

        } // namespace RLA
    } // namespace AV
} // namespace djv

