//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/Cineon.h>

#include <djvCore/FileIO.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                struct Read::Private
                {
                    ColorProfile colorProfile = ColorProfile::FilmPrint;
                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const std::string & fileName,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    FileSystem::FileIO io;
                    return _open(fileName, io);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    DJV_PRIVATE_PTR();
                    FileSystem::FileIO io;
                    const auto info = _open(fileName, io);
                    auto out = Image::Image::create(info.video[0].info);
                    out->setTags(info.tags);
                    const size_t size = std::min(out->getDataByteCount(), io.getSize() - io.getPos());
                    memcpy(out->getData(), io.mmapP(), size);
                    return out;
                }

                Info Read::_open(const std::string & fileName, FileSystem::FileIO & io)
                {
                    DJV_PRIVATE_PTR();
                    io.open(fileName, FileSystem::FileIO::Mode::Read);
                    Info info;
                    info.video.resize(1);
                    info.video[0].info.name = fileName;
                    read(io, info, p.colorProfile);
                    info.video[0].duration = _duration;
                    return info;
                }

            } // namespace Cineon
        } // namespace IO
    } // namespace AV
} // namespace djv

