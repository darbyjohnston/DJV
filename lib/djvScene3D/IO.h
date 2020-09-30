// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/FileInfo.h>
#include <djvSystem/ISystem.h>

#include <djvCore/RapidJSONFunc.h>
#include <djvCore/ValueObserver.h>

#include <future>
#include <set>

namespace djv
{
    namespace Scene3D
    {
        class Scene;

        //! This namespace provides I/O functionality.
        namespace IO
        {
            //! This class provides I/O information.
            class Info
            {
            public:
                Info();

                std::string fileName;

                bool operator == (const Info&) const;
            };

            //! This class provides an interface for I/O.
            class IIO : public std::enable_shared_from_this<IIO>
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            public:
                virtual ~IIO() = 0;

            protected:
                std::shared_ptr<System::LogSystem> _logSystem;
                std::shared_ptr<System::ResourceSystem> _resourceSystem;
                std::shared_ptr<System::TextSystem> _textSystem;
                System::File::Info _fileInfo;
            };

            //! This class provides an interface for reading.
            class IRead : public IIO
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            public:
                virtual ~IRead() = 0;

                virtual std::future<Info> getInfo() = 0;
                virtual std::future<std::shared_ptr<Scene> > getScene() = 0;
            };

            //! This class provides an interface for writing.
            class IWrite : public IIO
            {
            protected:
                void _init(
                    const System::File::Info&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            public:
                virtual ~IWrite() = 0;

                virtual std::future<bool> setScene(const std::shared_ptr<Scene>&) = 0;
            };

            //! This class provides an interface for I/O plugins.
            class IPlugin : public std::enable_shared_from_this<IPlugin>
            {
            protected:
                void _init(
                    const std::string& pluginName,
                    const std::string& pluginInfo,
                    const std::set<std::string>& fileExtensions,
                    const std::shared_ptr<System::Context>&);

            public:
                virtual ~IPlugin() = 0;

                const std::string& getPluginName() const;
                const std::string& getPluginInfo() const;
                const std::set<std::string>& getFileExtensions() const;

                virtual bool canRead(const System::File::Info&) const;
                virtual bool canWrite(const System::File::Info&) const;

                virtual rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const;

                //! Throws:
                //! - std::invalid_argument
                virtual void setOptions(const rapidjson::Value&);

                //! Throws:
                //! - System::File::Error
                virtual std::shared_ptr<IRead> read(const System::File::Info&) const;

                //! Throws:
                //! - System::File::Error
                virtual std::shared_ptr<IWrite> write(const System::File::Info&) const;

            protected:
                std::weak_ptr<System::Context> _context;
                std::shared_ptr<System::LogSystem> _logSystem;
                std::shared_ptr<System::ResourceSystem> _resourceSystem;
                std::shared_ptr<System::TextSystem> _textSystem;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            //! This class provides an I/O system.
            class IOSystem : public System::ISystem
            {
                DJV_NON_COPYABLE(IOSystem);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                IOSystem();

            public:
                ~IOSystem() override;

                static std::shared_ptr<IOSystem> create(const std::shared_ptr<System::Context>&);

                std::set<std::string> getPluginNames() const;
                std::set<std::string> getFileExtensions() const;

                rapidjson::Value getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType&) const;

                //! Throws:
                //! - std::invalid_argument
                void setOptions(const std::string& pluginName, const rapidjson::Value&);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeOptionsChanged() const;

                const std::set<std::string>& getSequenceExtensions() const;
                bool canRead(const System::File::Info&) const;
                bool canWrite(const System::File::Info&, const Info&) const;

                //! Throws:
                //! - System::File::Error
                std::shared_ptr<IRead> read(const System::File::Info&);

                //! Throws:
                //! - System::File::Error
                std::shared_ptr<IWrite> write(const System::File::Info&);

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/IOInline.h>
