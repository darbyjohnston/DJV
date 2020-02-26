//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/DPX.h>

#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace DPX
            {
                struct Write::Private
                {
                    Options options;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const FileSystem::FileInfo& fileInfo,
                    const Info & info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Image::Type Write::_getImageType(Image::Type) const
                {
                    return Image::Type::RGB_U10;
                }

                Image::Layout Write::_getImageLayout() const
                {
                    Image::Layout out;
                    out.endian = Memory::Endian::MSB;
                    out.alignment = 4;
                    return out;
                }
                
                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
                {
                    DJV_PRIVATE_PTR();
                    FileSystem::FileIO io;
                    io.open(fileName, FileSystem::FileIO::Mode::Write);
                    Info info;
                    info.video.push_back(image->getInfo());
                    info.tags = image->getTags();
                    write(
                        io,
                        info,
                        p.options.version,
                        p.options.endian,
                        _options.colorSpace.empty() ? Cineon::ColorProfile::Raw : Cineon::ColorProfile::FilmPrint);
                    io.write(image->getData(), image->getDataByteCount());
                    writeFinish(io);
                }

            } // namespace DPX
        } // namespace IO
    } // namespace AV
} // namespace djv

