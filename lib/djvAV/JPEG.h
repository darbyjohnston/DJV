// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

#if defined(DJV_PLATFORM_WINDOWS)
#include <basetsd.h>
#endif // DJV_PLATFORM_WINDOWS

extern "C"
{
#include <jpeglib.h>

} // extern "C"

#include <setjmp.h>

namespace djv
{
    namespace AV
    {
        //! This namespace provides Joint Photographic Experts Group (JPEG) image I/O.
        //!
        //! References:
        //! - http://www.ijg.org
        namespace JPEG
        {
            static const std::string pluginName = "JPEG";
            static const std::set<std::string> fileExtensions = { ".jpeg", ".jpg", ".jfif" };

            //! This struct provides the JPEG file I/O options.
            struct Options
            {
                int quality = 90;
                    
                bool operator == (const Options&) const;
            };

            //! This struct provides libjpeg error handling.
            struct JPEGErrorStruct
            {
                struct jpeg_error_mgr pub;
                std::vector<std::string> messages;
                jmp_buf jump;
            };

            //! This class provides the JPEG file reader.
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
                
            //! This class provides the JPEG file writer.
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
                    const Options&,
                    const std::shared_ptr<System::TextSystem>&,
                    const std::shared_ptr<System::ResourceSystem>&,
                    const std::shared_ptr<System::LogSystem>&);

            protected:
                Image::Type _getImageType(Image::Type) const override;
                void _write(const std::string& fileName, const std::shared_ptr<Image::Data>&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides the JPEG file I/O plugin.
            class Plugin : public IO::ISequencePlugin
            {
                DJV_NON_COPYABLE(Plugin);

            protected:
                Plugin();

            public:
                ~Plugin() override;

                static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                void setOptions(const rapidjson::Value&) override;

                std::shared_ptr<IO::IRead> read(const System::File::Info&, const IO::ReadOptions&) const override;
                std::shared_ptr<IO::IWrite> write(const System::File::Info&, const IO::Info&, const IO::WriteOptions&) const override;

            private:
                DJV_PRIVATE();
            };

            extern "C"
            {
                void djvJPEGError(j_common_ptr);
                void djvJPEGWarning(j_common_ptr, int);

            } // extern "C"

        } // namespace JPEG
    } // namespace AV

    rapidjson::Value toJSON(const AV::JPEG::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::JPEG::Options&);

} // namespace djv
