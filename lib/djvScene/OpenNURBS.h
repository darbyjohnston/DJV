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
                        const std::shared_ptr<Core::TextSystem>&,
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

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace OpenNURBS
        } // namespace IO
    } // namespace Scene
    
    rapidjson::Value toJSON(const Scene::IO::OpenNURBS::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Scene::IO::OpenNURBS::Options&);

} // namespace djv

