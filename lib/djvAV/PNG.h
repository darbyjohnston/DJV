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
        namespace IO
        {
            //! This namespace provides Portable Network Graphics (PNG) file I/O.
            //!
            //! References:
            //! - http://www.libpng.org
            namespace PNG
            {
                static const std::string pluginName = "PNG";
                static const std::set<std::string> fileExtensions = { ".png" };

                //! This struct provides a PNG error message.
                struct ErrorStruct
                {
                    std::vector<std::string> messages;
                };

                //! This class provides the PNG file reader.
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
                    std::shared_ptr<Image::Image> _readImage(const std::string& fileName) override;

                private:
                    class File;
                    Info _open(const std::string&, const std::shared_ptr<File>&);
                };
                
                //! This class provides the PNG file writer.
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
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    void _write(const std::string& fileName, const std::shared_ptr<Image::Image>&) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the PNG file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                    std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const System::File::Info&, const Info&, const WriteOptions&) const override;
                };

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

extern "C"
{
    void djvPngError(png_structp, png_const_charp);
    void djvPngWarning(png_structp, png_const_charp);

} // extern "C"

