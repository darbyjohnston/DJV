// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                struct Read::Private
                {
                    Transfer transfer = Transfer::FilmPrint;
                    Options options;
                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const Options& options,
                    const std::shared_ptr<TextSystem>& textSystem,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_p->options = options;
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string& fileName)
                {
                    auto io = FileSystem::FileIO::create();
                    return _open(fileName, io);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    auto io = FileSystem::FileIO::create();
                    const auto info = _open(fileName, io);
                    auto out = Cineon::Read::readImage(info, io);
                    out->setPluginName(pluginName);
                    return out;
                }

                Info Read::_open(const std::string& fileName, const std::shared_ptr<FileSystem::FileIO>& io)
                {
                    DJV_PRIVATE_PTR();
                    io->open(fileName, FileSystem::FileIO::Mode::Read);
                    Info info;
                    info.videoSpeed = _speed;
                    info.videoSequence = _sequence;
                    info.video.push_back(Image::Info());
                    DPX::read(io, info, p.transfer, _textSystem);
                    return info;
                }

            } // namespace DPX
        } // namespace IO
    } // namespace AV
} // namespace djv

