// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

#include <png.h>

namespace djv
{
    namespace AV
    {
        //! Portable Network Graphics (PNG) image I/O.
        //!
        //! References:
        //! - http://www.libpng.org
        namespace PNG
        {
            static const std::string pluginName = "PNG";
            static const std::set<std::string> fileExtensions = { ".png" };

            //! Error handling.
            struct ErrorStruct
            {
                std::vector<std::string> messages;
            };

            //! PNG reader.
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
                class File;
                IO::Info _open(const std::string&, const std::shared_ptr<File>&);
            };
                
            //! PNG writer.
            class Write : public IO::ISequenceWrite
            {
                DJV_NON_COPYABLE(Write);

            protected:
                Write();

            public:
                ~Write() override;

                static std::shared_ptr<Write> create(
                    const System::File::Info&,
                    const IO::Info&,
                    const IO::WriteOptions&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            protected:
                Image::Type _getImageType(Image::Type) const override;
                void _write(const std::string& fileName, const std::shared_ptr<Image::Data>&) override;

            private:
                DJV_PRIVATE();
            };

            //! PNG I/O plugin.
            class Plugin : public IO::ISequencePlugin
            {
                DJV_NON_COPYABLE(Plugin);

            protected:
                Plugin();

            public:
                static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<IO::IRead> read(const System::File::Info&, const IO::ReadOptions&) const override;
                std::shared_ptr<IO::IWrite> write(const System::File::Info&, const IO::Info&, const IO::WriteOptions&) const override;
            };

        } // namespace PNG
    } // namespace AV
} // namespace djv

extern "C"
{
    void djvPngError(png_structp, png_const_charp);
    void djvPngWarning(png_structp, png_const_charp);

} // extern "C"

