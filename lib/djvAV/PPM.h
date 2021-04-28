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

            //! This struct provides the PPM file I/O options.
            struct Options
            {
                Data data = Data::Binary;
                    
                bool operator == (const Options&) const;
            };

            //! This class provides the PPM file reader.
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
                IO::Info _readInfo(const std::string&) override;
                std::shared_ptr<Image::Data> _readImage(const std::string&) override;

            private:
                IO::Info _open(const std::string&, const std::shared_ptr<System::File::IO>&, Data&);
            };
                
            //! This class provides the PPM file writer.
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
                Image::Layout _getImageLayout() const override;
                void _write(const std::string& fileName, const std::shared_ptr<Image::Data>&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides the PPM file I/O plugin.
            class Plugin : public IO::ISequencePlugin
            {
                DJV_NON_COPYABLE(Plugin);

            protected:
                Plugin();

            public:
                static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                void setOptions(const rapidjson::Value&) override;

                std::shared_ptr<IO::IRead> read(const System::File::Info&, const IO::ReadOptions&) const override;
                std::shared_ptr<IO::IWrite> write(const System::File::Info&, const IO::Info&, const IO::WriteOptions&) const override;

            private:
                DJV_PRIVATE();
            };

        } // namespace PPM
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::PPM::Data);
    
    rapidjson::Value toJSON(const AV::PPM::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::PPM::Options&);

} // namespace djv
