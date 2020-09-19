// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/SequenceIO.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides NetPBM file I/O.
            //!
            //! References:
            //! - Netpbm, "PPM Format Specification"
            //!   http://netpbm.sourceforge.net/doc/ppm.html
            namespace PPM
            {
                static const std::string pluginName = "PPM";
                static const std::set<std::string> fileExtensions = { ".ppm" };

                //! This enumeration provides the PPM file data types.
                enum class Data
                {
                    ASCII,
                    Binary,

                    Count,
                    First = ASCII
                };
                DJV_ENUM_HELPERS(Data);

                //! This struct provides the PPM file I/O options.
                struct Options
                {
                    Data data = Data::Binary;
                    
                    bool operator == (const Options&) const;
                };

                //! Get the number of bytes in a scanline.
                size_t getScanlineByteCount(
                    int    width,
                    size_t channelCount,
                    size_t bitDepth);

                //! Read PPM file ASCII data.
                void readASCII(
                    const std::shared_ptr<System::File::IO>& io,
                    uint8_t* out,
                    size_t size,
                    size_t componentSize);

                //! Save PPM file ASCII data.
                size_t writeASCII(
                    const uint8_t * in,
                    char *          out,
                    size_t          size,
                    size_t          componentSize);

                //! This class provides the PPM file reader.
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
                    Info _readInfo(const std::string&) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string&) override;

                private:
                    Info _open(const std::string&, const std::shared_ptr<System::File::IO>&, Data&);
                };
                
                //! This class provides the PPM file writer.
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
                    void _write(const std::string& fileName, const std::shared_ptr<Image::Image>&) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the PPM file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
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

            } // namespace PPM
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::PPM::Data);
    
    rapidjson::Value toJSON(const AV::IO::PPM::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::PPM::Options&);

} // namespace djv
