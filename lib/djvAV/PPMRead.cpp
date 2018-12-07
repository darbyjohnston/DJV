//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/PPM.h>

#include <djvAV/FFmpeg.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PPM
            {
                namespace
                {
                    const size_t timeout = 10;

                } // namespace

                struct Read::Private
                {
                    Pixel::Info info;
                    std::promise<Info> infoPromise;
                    Data data = Data::First;
                    std::condition_variable queueCV;
                    Timestamp seek = -1;
                    std::thread thread;
                    std::atomic<bool> running;
                };

                void Read::_init(
                    const std::string & fileName,
                    const std::shared_ptr<Queue> & queue,
                    const std::shared_ptr<Core::Context> & context)
                {
                    IRead::_init(fileName, queue, context);
                    _p->thread = std::thread(
                        [this, fileName]
                    {
                        Core::FileInfo fileInfo(fileName);
                        fileInfo.evalSequence();

                        AV::Speed speed;
                        Duration duration = 0;
                        std::vector<Core::Frame::Number> frames;
                        Core::Frame::Index frameIndex = Core::Frame::Invalid;
                        if (fileInfo.isSequenceValid())
                        {
                            frames = Core::Frame::toFrames(fileInfo.getSequence());
                            if (frames.size())
                            {
                                frameIndex = 0;
                                duration = speed.getDuration() * frames.size();
                            }
                        }

                        while (_queue && _p->running)
                        {
                            bool read = false;
                            Timestamp seek = -1;
                            {
                                std::unique_lock<std::mutex> lock(_queue->getMutex());
                                if (_p->queueCV.wait_for(
                                    lock,
                                    std::chrono::milliseconds(timeout),
                                    [this]
                                {
                                    return (_queue->getVideoCount() < _queue->getVideoMax()) || _p->seek != -1;
                                }))
                                {
                                    read = true;
                                    if (_p->seek != -1)
                                    {
                                        seek = _p->seek;
                                        _p->seek = -1;
                                        _queue->setFinished(false);
                                        _queue->clear();
                                    }
                                }
                            }
                            if (seek != -1)
                            {

                            }
                            if (read)
                            {
                                std::shared_ptr<Image> image;
                                Timestamp pts = 0;
                                try
                                {
                                    Core::Frame::Number frameNumber = Core::Frame::Invalid;
                                    if (frameIndex != Core::Frame::Invalid)
                                    {
                                        frameNumber = Core::Frame::getFrame(frames, frameIndex);
                                        ++frameIndex;
                                    }
                                    auto fileName = fileInfo.getFileName(frameNumber);

                                    AVRational r;
                                    r.num = speed.getDuration();
                                    r.den = speed.getScale();
                                    pts = av_rescale_q(frameNumber * speed.getDuration(), r, FFmpeg::getTimeBaseQ());

                                    if (auto context = _context.lock())
                                    {
                                        std::stringstream ss;
                                        ss << _fileName << ", read frame: " << pts;
                                        context->log("djv::AV::IO::PPM", ss.str());
                                    }

                                    auto io = _open(fileName);
                                    if (Core::Frame::Invalid == frameIndex || 0 == frameIndex)
                                    {
                                        _p->infoPromise.set_value(Info(fileName, VideoInfo(_p->info, speed, duration), AudioInfo()));
                                    }
                                    switch (_p->data)
                                    {
                                    case Data::ASCII:
                                    {
                                        image = Image::create(_p->info);
                                        const size_t channelCount = Pixel::getChannelCount(_p->info.getType());
                                        const size_t bitDepth = Pixel::getBitDepth(_p->info.getType());
                                        for (int y = 0; y < _p->info.getHeight(); ++y)
                                        {
                                            readASCII(io, image->getData(y), _p->info.getWidth() * channelCount, bitDepth);
                                        }
                                        break;
                                    }
                                    case Data::Binary:
                                        image = Image::create(_p->info);
                                        io.read(image->getData(), _p->info.getDataByteCount());
                                        break;
                                    }
                                }
                                catch (const Core::Error & error)
                                {
                                    if (auto context = _context.lock())
                                    {
                                        context->log("djvAV::IO::PPM::Read", error.what(), Core::LogLevel::Error);
                                    }
                                }
                                if (image)
                                {
                                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                                    _queue->addVideo(pts, image);
                                }
                                if (Core::Frame::Invalid == frameIndex || frameIndex >= static_cast<Core::Frame::Index>(frames.size()))
                                {
                                    std::lock_guard<std::mutex> lock(_queue->getMutex());
                                    _queue->setFinished(true);
                                }
                            }
                        }
                    });
                }

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {
                    _p->thread.join();
                }

                std::shared_ptr<Read> Read::create(const std::string & fileName, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileName, queue, context);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return _p->infoPromise.get_future();
                }

                Core::FileIO Read::_open(const std::string& fileName)
                {
                    Core::FileIO io;
                    io.open(fileName, Core::FileIO::Mode::Read);

                    char magic[] = { 0, 0, 0 };
                    io.read(magic, 2);
                    if (magic[0] != 'P')
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    switch (magic[1])
                    {
                    case '2':
                    case '3':
                    case '5':
                    case '6': break;
                    default:
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    }
                    const int ppmType = magic[1] - '0';
                    _p->data = (2 == ppmType || 3 == ppmType) ? Data::ASCII : Data::Binary;

                    size_t channelCount = 0;
                    switch (ppmType)
                    {
                    case 2:
                    case 5: channelCount = 1; break;
                    case 3:
                    case 6: channelCount = 3; break;
                    }
                    char tmp[Core::String::cStringLength] = "";
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int w = std::stoi(tmp);
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int h = std::stoi(tmp);
                    Core::FileIO::readWord(io, tmp, Core::String::cStringLength);
                    const int maxValue = std::stoi(tmp);
                    const size_t bitDepth = maxValue < 256 ? 8 : 16;
                    const auto pixelType = Pixel::getIntType(channelCount, bitDepth);
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << " cannot open: " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    Pixel::Layout layout;
                    layout.setEndian(_p->data != Data::ASCII ? Core::Memory::Endian::MSB : Core::Memory::getEndian());
                    _p->info = Pixel::Info(w, h, pixelType, layout);

                    return io;
                }

            } // namespace PPM
        } // namespace IO
    } // namespace AV
} // namespace djv

