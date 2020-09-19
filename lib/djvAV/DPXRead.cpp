// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/DPX.h>

#include <djvAV/CineonFunc.h>
#include <djvAV/DPXFunc.h>

#include <djvSystem/FileIO.h>

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
                    const System::File::Info& fileInfo,
                    const ReadOptions& readOptions,
                    const Options& options,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_p->options = options;
                    out->_init(fileInfo, readOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string& fileName)
                {
                    auto io = System::File::IO::create();
                    return _open(fileName, io);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string& fileName)
                {
                    auto io = System::File::IO::create();
                    const auto info = _open(fileName, io);
                    auto out = Cineon::Read::readImage(info, io);
                    out->setPluginName(pluginName);
                    return out;
                }

                Info Read::_open(const std::string& fileName, const std::shared_ptr<System::File::IO>& io)
                {
                    DJV_PRIVATE_PTR();
                    io->open(fileName, System::File::IO::Mode::Read);
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

