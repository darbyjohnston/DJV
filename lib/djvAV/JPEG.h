// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

#include <djvCore/RapidJSON.h>
#include <djvCore/String.h>

#if defined(DJV_PLATFORM_WINDOWS)
#include <basetsd.h>
#endif // DJV_PLATFORM_WINDOWS

#include <jpeglib.h>

#include <setjmp.h>

namespace djv
{
    namespace AV
    {
        namespace IO
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
                };

                //! This struct provides libjpeg error handling.
                struct JPEGErrorStruct
                {
                    struct jpeg_error_mgr pub;
                    std::vector<std::string> messages;
                    jmp_buf jump;
                };

                //! This class provides the JPEG file reader.
                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const ReadOptions&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Info _readInfo(const std::string& fileName) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string& fileName) override;

                private:
                    class File;
                    Info _open(const std::string&, const std::shared_ptr<File>&);
                };
                
                //! This class provides the JPEG file writer.
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const Core::FileSystem::FileInfo&,
                        const Info&,
                        const WriteOptions&,
                        const Options&,
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    void _write(const std::string& fileName, const std::shared_ptr<Image::Image>&) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the JPEG file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    ~Plugin() override;

                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info&, const WriteOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

                extern "C"
                {
                    void djvJPEGError(j_common_ptr);
                    void djvJPEGWarning(j_common_ptr, int);

                } // extern "C"

            } // namespace JPEG
        } // namespace IO
    } // namespace AV

    rapidjson::Value toJSON(const AV::IO::JPEG::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::JPEG::Options&);

} // namespace djv
