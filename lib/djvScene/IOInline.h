// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            inline bool Info::operator == (const Info & other) const
            {
                return fileName == other.fileName;
            }

            inline const std::string& IPlugin::getPluginName() const
            {
                return _pluginName;
            }

            inline const std::string& IPlugin::getPluginInfo() const
            {
                return _pluginInfo;
            }

            inline const std::set<std::string>& IPlugin::getFileExtensions() const
            {
                return _fileExtensions;
            }

        } // namespace IO
    } // namespace Scene
} // namespace djv
