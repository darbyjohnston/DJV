// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/IOPlugin.h>

#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
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

                //! \name Information
                ///@{

                std::set<std::string> getPluginNames() const;
                std::set<std::string> getFileExtensions() const;

                ///@}

                //! \name Options
                ///@{

                rapidjson::Value getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType&) const;

                //! Throws:
                //! - std::exception
                void setOptions(const std::string& pluginName, const rapidjson::Value&);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeOptionsChanged() const;

                ///@}
                
                //! \name Sequences
                ///@{

                const std::set<std::string>& getSequenceExtensions() const;
                const std::set<std::string>& getNonSequenceExtensions() const;
                bool canSequence(const System::File::Info&) const;

                ///@}
                
                //! \name Read
                ///@{

                bool canRead(const System::File::Info&) const;

                //! Throws:
                //! - std::exception
                std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions& = ReadOptions());

                ///@}

                //! \name Write
                ///@{

                bool canWrite(const System::File::Info&, const Info&) const;

                //! Throws:
                //! - std::exception
                std::shared_ptr<IWrite> write(const System::File::Info&, const Info&, const WriteOptions& = WriteOptions());

                ///@}

            private:
                DJV_PRIVATE();
            };

        } // namespace IO
    } // namespace AV
} // namespace djv
