// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IOPlugin.h>

#include <djvCore/ISystem.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem

    } // namespace Core

    namespace AV
    {
        namespace IO
        {
            //! This class provides an I/O system.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                ~System() override;

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                std::set<std::string> getPluginNames() const;
                std::set<std::string> getFileExtensions() const;

                rapidjson::Value getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType&) const;

                //! Throws:
                //! - std::exception
                void setOptions(const std::string& pluginName, const rapidjson::Value&);

                std::shared_ptr<Core::IValueSubject<bool> > observeOptionsChanged() const;

                const std::set<std::string>& getSequenceExtensions() const;
                const std::set<std::string>& getNonSequenceExtensions() const;
                bool canSequence(const Core::FileSystem::FileInfo&) const;
                bool canRead(const Core::FileSystem::FileInfo&) const;
                bool canWrite(const Core::FileSystem::FileInfo&, const Info&) const;

                //! Throws:
                //! - std::exception
                std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions& = ReadOptions());

                //! Throws:
                //! - std::exception
                std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info&, const WriteOptions& = WriteOptions());

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace AV
} // namespace djv
