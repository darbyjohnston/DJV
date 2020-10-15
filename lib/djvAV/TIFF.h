// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

#include <tiffio.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides Tagged Image File Format (TIFF) I/O.
            //!
            //! References:
            //! - http://www.libtiff.org
            namespace TIFF
            {
                static const std::string pluginName = "TIFF";
                static const std::set<std::string> fileExtensions = { ".tiff", ".tif" };

                //! This enumeration provides the TIFF file compression types.
                enum class Compression
                {
                    None,
                    RLE,
                    LZW,

                    Count,
                    First
                };

                //! This struct provides the TIFF file I/O options.
                struct Options
                {
                    Compression compression = Compression::LZW;
                    
                    bool operator == (const Options&) const;
                };

                //! This class provides the TIFF file reader.
                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const System::File::Info&,
                        const ReadOptions&,
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);

                protected:
                    Info _readInfo(const std::string& fileName) override;
                    std::shared_ptr<Image::Data> _readImage(const std::string& fileName) override;

                private:
                    struct File;
                    Info _open(const std::string&, File&);
                };
                
                //! This class provides the TIFF file writer.
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const System::File::Info&,
                        const Info&,
                        const WriteOptions&,
                        const Options&,
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    Image::Layout _getImageLayout() const override;
                    void _write(const std::string& fileName, const std::shared_ptr<Image::Data>&) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the TIFF file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    void _init(const std::shared_ptr<System::Context>&);
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const System::File::Info&, const Info&, const WriteOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv
