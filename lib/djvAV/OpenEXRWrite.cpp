// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenEXR.h>

#include <djvAV/OpenEXRFunc.h>

#include <ImfCompressionAttribute.h>
#include <ImfOutputFile.h>
#include <ImfStandardAttributes.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
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
                    const System::File::Info& fileInfo,
                    const Info& info,
                    const WriteOptions& writeOptions,
                    const Options& options,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<System::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->options = options;
                    out->_init(fileInfo, info, writeOptions, textSystem, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    struct File
                    {
                        ~File()
                        {
                        }
                    };
                }

                Image::Type Write::_getImageType(Image::Type value) const
                {
                    Image::Type out = Image::Type::None;
                    switch (value)
                    {
                    case Image::Type::L_U8:
                    case Image::Type::L_U16:    out = Image::Type::L_F16; break;
                    case Image::Type::L_F16:
                    case Image::Type::L_U32:
                    case Image::Type::L_F32:    out = value; break;
                    case Image::Type::LA_U8:    out = Image::Type::LA_F16; break;
                    case Image::Type::LA_U16:
                    case Image::Type::LA_U32:
                    case Image::Type::LA_F32:   out = value; break;
                    case Image::Type::RGB_U8:
                    case Image::Type::RGB_U10:
                    case Image::Type::RGB_U16:  out = Image::Type::RGB_F16; break;
                    case Image::Type::RGB_U32:
                    case Image::Type::RGB_F32:  out = value; break;
                    case Image::Type::RGBA_U8:
                    case Image::Type::RGBA_U16: out = Image::Type::RGBA_F16; break;
                    case Image::Type::RGBA_U32:
                    case Image::Type::RGBA_F32: out = value; break;
                    default: break;
                    }
                    return out;
                }

                Image::Layout Write::_getImageLayout() const
                {
                    Image::Layout out;
                    return out;
                }

                void Write::_write(const std::string& fileName, const std::shared_ptr<Image::Data>& image)
                {
                    DJV_PRIVATE_PTR();
                    const Image::Info& info = image->getInfo();
                    Imf::Header header(info.size.w, info.size.h);
                    std::vector<std::string> channels;
                    const uint8_t c = Image::getChannelCount(info.type);
                    switch (c)
                    {
                    case 1:
                        channels.push_back("Y");
                        break;
                    case 2:
                        channels.push_back("Y");
                        channels.push_back("A");
                        break;
                    case 3:
                        channels.push_back("R");
                        channels.push_back("G");
                        channels.push_back("B");
                        break;
                    case 4:
                        channels.push_back("R");
                        channels.push_back("G");
                        channels.push_back("B");
                        channels.push_back("A");
                        break;
                    }
                    for (size_t i = 0; i < channels.size(); ++i)
                    {
                        header.channels().insert(channels[i], toImf(Image::getDataType(info.type)));
                    }

                    Imf::CompressionAttribute compression;
                    switch (p.options.compression)
                    {
                    case Compression::None:
                        compression = Imf::NO_COMPRESSION;
                        break;
                    case Compression::RLE:
                        compression = Imf::RLE_COMPRESSION;
                        break;
                    case Compression::ZIPS:
                        compression = Imf::ZIPS_COMPRESSION;
                        break;
                    case Compression::ZIP:
                        compression = Imf::ZIP_COMPRESSION;
                        break;
                    case Compression::PIZ:
                        compression = Imf::PIZ_COMPRESSION;
                        break;
                    case Compression::PXR24:
                        compression = Imf::PXR24_COMPRESSION;
                        break;
                    case Compression::B44:
                        compression = Imf::B44_COMPRESSION;
                        break;
                    case Compression::B44A:
                        compression = Imf::B44A_COMPRESSION;
                        break;
                    case Compression::DWAA:
                        compression = Imf::DWAA_COMPRESSION;
                        break;
                    case Compression::DWAB:
                        compression = Imf::DWAB_COMPRESSION;
                        break;
                    default: break;
                    }
                    header.insert(Imf::CompressionAttribute::staticTypeName(), compression);
                    addDwaCompressionLevel(header, p.options.dwaCompressionLevel);
                    writeTags(image->getTags(), _info.videoSpeed, header);

                    auto out = std::unique_ptr<Imf::OutputFile>(new Imf::OutputFile(fileName.c_str(), header));
                    const uint8_t* data = image->getData();
                    const uint8_t cb = Image::getByteCount(Image::getDataType(info.type));
                    Imf::FrameBuffer frameBuffer;
                    for (uint8_t i = 0; i < c; ++i)
                    {
                        frameBuffer.insert(
                            channels[i],
                            Imf::Slice(
                                toImf(Image::getDataType(info.type)),
                                (char*)data + i * cb,
                                c * cb,
                                info.size.w * c * cb,
                                1,
                                1,
                                0.f));
                    }
                    out->setFrameBuffer(frameBuffer);
                    out->writePixels(info.size.h);
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

