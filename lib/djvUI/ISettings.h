// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSON.h>

#include <memory>

namespace djv
{
    namespace System
    {
        class Context;
    
    } // namespace System

    namespace UI
    {
        //! This namespace provides user settings.
        namespace Settings
        {
            //! This class provides the interface for saving and restoring user settings.
            class ISettings : public std::enable_shared_from_this<ISettings>
            {
            protected:
                void _init(const std::string& name, const std::shared_ptr<System::Context>&);

            public:
                ISettings();
                virtual ~ISettings() = 0;

                // Get the context.
                const std::weak_ptr<System::Context>& getContext() const;

                // Get the settings name.
                const std::string& getName() const;

                //! Load the settings from the given JSON. This function is called by the settings system.
                virtual void load(const rapidjson::Value&) = 0;

                //! Save the settings to JSON. This function is called by the settings system.
                virtual rapidjson::Value save(rapidjson::Document::AllocatorType&) = 0;

            protected:
                //! \todo This function needs to be called by derived classes at the end of their _init() function.
                void _load();

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv
