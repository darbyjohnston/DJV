// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            inline const std::vector<std::shared_ptr<ISettings> >& System::getSettings() const
            {
                return _settings;
            }

            template<typename T>
            inline std::shared_ptr<T> System::getSettingsT() const
            {
                for (const auto& i : _settings)
                {
                    if (auto settings = std::dynamic_pointer_cast<T>(i))
                    {
                        return settings;
                    }
                }
                return nullptr;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv
