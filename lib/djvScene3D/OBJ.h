// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IO.h>

namespace djv
{
    namespace Scene3D
    {
        //! Wavefront OBJ I/O.
        namespace OBJ
        {
            static const std::string pluginName = "OBJ";
            static const std::set<std::string> fileExtensions = { ".obj" };

            //! OBJ I/O options.
            struct Options
            {
            };

            //! OBJ reader.
            class Read : public IO::IRead
            {
                DJV_NON_COPYABLE(Read);

            protected:
                Read();

            public:
                ~Read() override;

                static std::shared_ptr<Read> create(
                    const System::File::Info&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

                std::future<IO::Info> getInfo() override;
                std::future<std::shared_ptr<Scene> > getScene() override;

            private:
                DJV_PRIVATE();
            };

            //! OBJ I/O plugin.
            class Plugin : public IO::IPlugin
            {
                DJV_NON_COPYABLE(Plugin);

            protected:
                Plugin();

            public:
                ~Plugin() override;

                static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                void setOptions(const rapidjson::Value&) override;

                std::shared_ptr<IO::IRead> read(const System::File::Info&) const override;

            private:
                DJV_PRIVATE();
            };

        } // namespace OBJ
    } // namespace Scene3D
    
    rapidjson::Value toJSON(const Scene3D::OBJ::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Scene3D::OBJ::Options&);

} // namespace djv

