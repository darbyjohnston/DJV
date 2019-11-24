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

#include <djvCore/FileInfo.h>
#include <djvCore/ISystem.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/ValueObserver.h>

#include <future>
#include <set>

namespace djv
{
    namespace Scene
    {
        class Scene;

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
                    const Core::FileSystem::FileInfo&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

            public:
                virtual ~IIO() = 0;

            protected:
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                Core::FileSystem::FileInfo _fileInfo;
            };

            //! This class provides an interface for reading.
            class IRead : public IIO
            {
            protected:
                void _init(
                    const Core::FileSystem::FileInfo&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

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
                    const Core::FileSystem::FileInfo&,
                    const std::shared_ptr<Core::ResourceSystem>&,
                    const std::shared_ptr<Core::LogSystem>&);

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
                    const std::shared_ptr<Core::Context>&);

            public:
                virtual ~IPlugin() = 0;

                const std::string& getPluginName() const;
                const std::string& getPluginInfo() const;
                const std::set<std::string>& getFileExtensions() const;

                virtual bool canRead(const Core::FileSystem::FileInfo&) const;
                virtual bool canWrite(const Core::FileSystem::FileInfo&) const;

                virtual picojson::value getOptions() const;

                //! Throws:
                //! - std::invalid_argument
                virtual void setOptions(const picojson::value&);

                //! Throws:
                //! - Core::FileSystem::Error
                virtual std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&) const;

                //! Throws:
                //! - Core::FileSystem::Error
                virtual std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&) const;

            protected:
                std::weak_ptr<Core::Context> _context;
                std::shared_ptr<Core::LogSystem> _logSystem;
                std::shared_ptr<Core::ResourceSystem> _resourceSystem;
                std::string _pluginName;
                std::string _pluginInfo;
                std::set<std::string> _fileExtensions;
            };

            //! This class provides an I/O system.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                virtual ~System();

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                std::set<std::string> getPluginNames() const;
                std::set<std::string> getFileExtensions() const;

                picojson::value getOptions(const std::string& pluginName) const;

                //! Throws:
                //! - std::invalid_argument
                void setOptions(const std::string& pluginName, const picojson::value&);

                std::shared_ptr<Core::IValueSubject<bool> > observeOptionsChanged() const;

                const std::set<std::string>& getSequenceExtensions() const;
                bool canRead(const Core::FileSystem::FileInfo&) const;
                bool canWrite(const Core::FileSystem::FileInfo&, const Info&) const;

                //! Throws:
                //! - Core::FileSystem::Error
                std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&);

                //! Throws:
                //! - Core::FileSystem::Error
                std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&);

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace Scene
} // namespace djv

#include <djvScene/IOInline.h>
