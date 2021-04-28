// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class IO;

        } // namespace File
    } // namespace System

    namespace AV
    {
        //! Targa image I/O.
        //!
        //! References:
        //! - James D. Murray, William vanRyper, "Encyclopedia of AV File Formats,
        //!   Second Edition"
        namespace Targa
        {
            static const std::string pluginName = "Targa";
            static const std::set<std::string> fileExtensions = { ".tga" };

            //! Targa reader.
            class Read : public IO::ISequenceRead
            {
                DJV_NON_COPYABLE(Read);

            protected:
                Read();

            public:
                ~Read() override;

                static std::shared_ptr<Read> create(
                    const System::File::Info&,
                    const IO::ReadOptions&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            protected:
                IO::Info _readInfo(const std::string& fileName) override;
                std::shared_ptr<Image::Data> _readImage(const std::string& fileName) override;

            private:
                IO::Info _open(const std::string&, const std::shared_ptr<System::File::IO>&);

                bool _bgr = false;
                bool _compression = false;
            };
                
            //! Targa I/O plugin.
            class Plugin : public IO::ISequencePlugin
            {
                DJV_NON_COPYABLE(Plugin);

            protected:
                Plugin();

            public:
                static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<IO::IRead> read(const System::File::Info&, const IO::ReadOptions&) const override;
            };

        } // namespace Targa
    } // namespace AV
} // namespace djv
