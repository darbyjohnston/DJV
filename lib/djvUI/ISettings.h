// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/PicoJSON.h>

#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace UI
    {
        //! This namespace provides user settings.
        namespace Settings
        {
            //! This class provides an interface for saving and restoring user settings.
            class ISettings : public std::enable_shared_from_this<ISettings>
            {
            protected:
                void _init(const std::string & name, const std::shared_ptr<Core::Context>&);

            public:
                ISettings();
                virtual ~ISettings() = 0;

                // Get the context.
                const std::weak_ptr<Core::Context>& getContext() const;

                // Get the settings name.
                const std::string & getName() const;

                //! Load the settings from the given JSON. This function is called by the settings system.
                virtual void load(const picojson::value &) = 0;

                //! Save the settings to JSON. This function is called by the settings system.
                virtual picojson::value save() = 0;

            protected:
                //! \todo This function needs to be called by derived classes at the end of their _init() function.
                void _load();

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
