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
            //! This namespace provides Wavefront RLA image I/O.
            //!
            //! References:
            //! - James D. Murray, William vanRyper, "Encyclopedia of AV File Formats,
            //!   Second Edition"
            namespace RLA
            {
                static const std::string pluginName = "RLA";
                static const std::set<std::string> fileExtensions = { ".rla", ".rpf" };

                //! This class provides the RLA file reader.
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
                    Info _open(const std::string&, const std::shared_ptr<System::File::IO>&);

                    std::vector<int32_t> _rleOffset;
                };

                //! This class provides the RLA file I/O plugin.
                class Plugin : public ISequencePlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                    std::shared_ptr<IRead> read(const System::File::Info&, const ReadOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace RLA
        } // namespace IO
    } // namespace AV
} // namespace djv
