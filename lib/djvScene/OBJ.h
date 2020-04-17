// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/IO.h>

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            //! This namespace provides OBJ file I/O.
            namespace OBJ
            {
                static const std::string pluginName = "OBJ";
                static const std::set<std::string> fileExtensions = { ".obj" };

                //! This struct provides the OBJ file I/O options.
                struct Options
                {
                };

                //! This class provides the OBJ file reader.
                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                    std::future<Info> getInfo() override;
                    std::future<std::shared_ptr<Scene> > getScene() override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the OBJ file I/O plugin.
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

            } // namespace OBJ
        } // namespace IO
    } // namespace Scene
    
    picojson::value toJSON(const Scene::IO::OBJ::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, Scene::IO::OBJ::Options&);

} // namespace djv

