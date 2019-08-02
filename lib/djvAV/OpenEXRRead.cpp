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

#include <djvAV/OpenEXR.h>

#include <djvCore/FileIO.h>

#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfInputFile.h>
#include <ImfRgbaYca.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
#if defined(DJV_MMAP)
                struct MemoryMappedIStream::Private
                {
                    FileSystem::FileIO  f;
                    uint64_t            size    = 0;
                    uint64_t            pos     = 0;
                    char*               p       = nullptr;
                };

                MemoryMappedIStream::MemoryMappedIStream(const char fileName[]) :
                    IStream(fileName),
                    _p(new Private)
                {
                    DJV_PRIVATE_PTR();
                    p.f.open(fileName, FileSystem::FileIO::Mode::Read);
                    p.size = p.f.getSize();
                    p.p = (char*)(p.f.mmapP());
                }

                MemoryMappedIStream::~MemoryMappedIStream()
                {}

                bool MemoryMappedIStream::isMemoryMapped() const
                {
                    return true;
                }

                char* MemoryMappedIStream::readMemoryMapped(int n)
                {
                    DJV_PRIVATE_PTR();
                    if (p.pos >= p.size)
                        throw std::runtime_error("Error reading OpenEXR file.");
                    if (p.pos + n > p.size)
                        throw std::runtime_error("Error reading OpenEXR file.");
                    char* out = p.p + p.pos;
                    p.pos += n;
                    return out;
                }

                bool MemoryMappedIStream::read(char c[], int n)
                {
                    DJV_PRIVATE_PTR();
                    if (p.pos >= p.size)
                        throw std::runtime_error("Error reading OpenEXR file.");
                    if (p.pos + n > p.size)
                        throw std::runtime_error("Error reading OpenEXR file.");
                    memcpy(c, p.p + p.pos, n);
                    p.pos += n;
                    return p.pos < p.size;
                }

                Imf::Int64 MemoryMappedIStream::tellg()
                {
                    return _p->pos;
                }

                void MemoryMappedIStream::seekg(Imf::Int64 pos)
                {
                    _p->pos = pos;
                }
#endif // DJV_MMAP

                struct Read::File
                {
                    ~File()
                    {
                    }

                    std::unique_ptr<MemoryMappedIStream> s;
                    std::unique_ptr<Imf::InputFile>      f;
                    BBox2i                               displayWindow;
                    BBox2i                               dataWindow;
                    BBox2i                               intersectedWindow;
                    std::vector<OpenEXR::Layer>          layers;
                    bool                                 fast              = false;
                };

                struct Read::Private
                {
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
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_p->options = options;
                    out->_init(fileInfo, readOptions, resourceSystem, logSystem);
                    return out;
                }

                Info Read::_readInfo(const std::string & fileName)
                {
                    File f;
                    return _open(fileName, f);
                }

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    File f;
                    Info info = _open(fileName, f);
                    Image::Info imageInfo = info.video[std::min(_options.layer, info.video.size() - 1)].info;
                    std::shared_ptr<Image::Image> out = Image::Image::create(imageInfo);
                    out->setTags(info.tags);
                    out->setColorSpace("lnh");
                    const uint8_t channels = Image::getChannelCount(imageInfo.type);
                    const uint8_t channelByteCount = Image::getByteCount(getDataType(imageInfo.type));
                    const uint8_t cb = channels * channelByteCount;
                    const uint16_t scb = imageInfo.size.w * channels * channelByteCount;
                    if (f.fast)
                    {
                        Imf::FrameBuffer frameBuffer;
                        for (uint8_t c = 0; c < channels; ++c)
                        {
                            const std::string& name = f.layers[_options.layer].channels[c].name;
                            const glm::ivec2& sampling = f.layers[_options.layer].channels[c].sampling;
                            frameBuffer.insert(
                                name.c_str(),
                                Imf::Slice(
                                    toImf(Image::getDataType(imageInfo.type)),
                                    (char*)out->getData() + (static_cast<size_t>(c) * static_cast<size_t>(channelByteCount)),
                                    cb,
                                    scb,
                                    sampling.x,
                                    sampling.y,
                                    0.f));
                        }
                        f.f->setFrameBuffer(frameBuffer);
                        f.f->readPixels(f.displayWindow.min.y, f.displayWindow.max.y);
                    }
                    else
                    {
                        Imf::FrameBuffer frameBuffer;
                        std::vector<char> buf(f.dataWindow.w() * cb);
                        for (int c = 0; c < channels; ++c)
                        {
                            const std::string& name = f.layers[_options.layer].channels[c].name;
                            const glm::ivec2& sampling = f.layers[_options.layer].channels[c].sampling;
                            frameBuffer.insert(
                                name.c_str(),
                                Imf::Slice(
                                    toImf(Image::getDataType(imageInfo.type)),
                                    buf.data() - (f.dataWindow.min.x * cb) + (c * channelByteCount),
                                    cb,
                                    0,
                                    sampling.x,
                                    sampling.y,
                                    0.f));
                        }
                        f.f->setFrameBuffer(frameBuffer);
                        for (int y = f.displayWindow.min.y; y <= f.displayWindow.max.y; ++y)
                        {
                            uint8_t* p = out->getData() + ((y - f.displayWindow.min.y) * scb);
                            uint8_t* end = p + scb;
                            if (y >= f.intersectedWindow.min.y && y <= f.intersectedWindow.max.y)
                            {
                                size_t size = (f.intersectedWindow.min.x - f.displayWindow.min.x) * cb;
                                memset(p, 0, size);
                                p += size;
                                size = f.intersectedWindow.w() * cb;
                                f.f->readPixels(y, y);
                                memcpy(
                                    p,
                                    buf.data() + std::max(f.displayWindow.min.x - f.dataWindow.min.x, 0) * cb,
                                    size);
                                p += size;
                            }
                            memset(p, 0, end - p);
                        }
                    }
                    return out;
                }

                Info Read::_open(const std::string & fileName, File & f)
                {
                    DJV_PRIVATE_PTR();

                    Info out;

                    // Open the file.
#if defined(DJV_MMAP)
                    f.s.reset(new MemoryMappedIStream(fileName.c_str()));
                    f.f.reset(new Imf::InputFile(*f.s.get()));
#else // DJV_MMAP
                    f.f.reset(new Imf::InputFile(fileName.c_str()));
#endif // DJV_MMAP

                    // Get the display and data windows.
                    f.displayWindow = fromImath(f.f->header().displayWindow());
                    f.dataWindow = fromImath(f.f->header().dataWindow());
                    f.intersectedWindow = f.displayWindow.intersect(f.dataWindow);
                    f.fast = f.displayWindow == f.dataWindow;

                    // Get the tags.
                    readTags(f.f->header(), out.tags, _speed);

                    // Get the layers.
                    f.layers = getLayers(f.f->header().channels(), p.options.channels);
                    out.fileName = fileName;
                    out.video.resize(f.layers.size());
                    for (size_t i = 0; i < f.layers.size(); ++i)
                    {
                        const auto& layer = f.layers[i];
                        const glm::ivec2 sampling(layer.channels[0].sampling.x, layer.channels[0].sampling.y);
                        if (sampling.x != 1 || sampling.y != 1)
                            f.fast = false;
                        auto& info = out.video[i].info;
                        info.name = layer.name;
                        info.size.w = f.displayWindow.w();
                        info.size.h = f.displayWindow.h();
                        info.pixelAspectRatio = f.f->header().pixelAspectRatio();
                        switch (layer.channels[0].type)
                        {
                        case Image::DataType::F16:
                        case Image::DataType::F32:
                            info.type = Image::getFloatType(layer.channels.size(), Image::getBitDepth(layer.channels[0].type));
                            break;
                        case Image::DataType::U32:
                            info.type = Image::getIntType(layer.channels.size(), Image::getBitDepth(layer.channels[0].type));
                            break;
                        default: break;
                        }
                        if (Image::Type::None == info.type)
                        {
                            throw std::runtime_error(DJV_TEXT("Unsupported image type."));
                        }
                        out.video[i].sequence = _sequence;
                        out.video[i].speed = _speed;
                    }

                    return out;
                }

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV
} // namespace djv

