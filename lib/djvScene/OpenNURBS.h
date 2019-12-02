//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvScene/IO.h>

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            //! This namespace provides OpenNURBS file I/O.
            namespace OpenNURBS
            {
                static const std::string pluginName = "OpenNURBS";
                static const std::set<std::string> fileExtensions = { ".3dm" };

                //! This struct provides the OpenNURBS file I/O options.
                struct Options
                {
                };

                //! This class provides the OpenNURBS file reader.
                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                    std::future<Info> getInfo() override;
                    std::future<std::shared_ptr<Scene> > getScene() override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the OpenNURBS file I/O plugin.
                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    ~Plugin() override;

                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace OpenNURBS
        } // namespace IO
    } // namespace Scene
    
    picojson::value toJSON(const Scene::IO::OpenNURBS::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, Scene::IO::OpenNURBS::Options&);

} // namespace djv

