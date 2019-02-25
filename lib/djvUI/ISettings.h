//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/ValueObserver.h>

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
                void _init(const std::string& name, Core::Context *);

            public:
                ISettings();
                virtual ~ISettings() = 0;

                // Get the context.
                Core::Context * getContext() const;

                // Get the settings name.
                const std::string& getName() const;

                //! Load the settings from the given JSON. This function is called by the settings system.
                virtual void load(const picojson::value&) = 0;

                //! Save the settings to JSON. This function is called by the settings system.
                virtual picojson::value save() = 0;

            protected:
                //! \todo This function needs to be called by derived classes at the end of their _init() function.
                void _load();

                template<typename T>
                inline void _read(const std::string& name, const picojson::object&, T&);
                template<typename T>
                inline void _read(const std::string& name, const picojson::object&, std::shared_ptr<Core::ValueSubject<T> >&);
                template<typename T>
                inline void _read(const std::string& name, const picojson::object&, std::shared_ptr<Core::ListSubject<T> >&);
                template<typename T, typename U>
                inline void _read(const std::string& name, const picojson::object&, std::shared_ptr<Core::MapSubject<T, U> >&);

                template<typename T>
                inline void _write(const std::string& name, const T&, picojson::object&);

                void _readError(const std::string& name, const std::string& what);

			private:
				DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

#include <djvUI/ISettingsInline.h>
