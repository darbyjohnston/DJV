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

#include <djvCore/Path.h>
#include <djvCore/ISystem.h>

#include <map>

namespace djv
{
    namespace Core
    {
        //! This class provides the location of resources.
        //!
        //! By default the application root is used for bundled resources. This
        //! may be overridden with the DJV_RESOURCE_PATH environment variable.
        //!
        //! By default log files and settings are written to "$HOME/Documents/DJV".
        //! This may be overridden with the DJV_DOCUMENTS_PATH environment variable.
        class ResourceSystem : public ISystem
        {
            DJV_NON_COPYABLE(ResourceSystem);
            void _init(const std::string & argv0, Context *);
            ResourceSystem();
            
        public:
            virtual ~ResourceSystem();

            // Create a resource system.
            static std::shared_ptr<ResourceSystem> create(const std::string & argv0, Context *);

            //! Get a resource path.
            FileSystem::Path getPath(FileSystem::ResourcePath) const;

        private:
            static FileSystem::Path _getDocumentsPath();

            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv

