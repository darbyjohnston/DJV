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

#include <djvAVTest/ImageDataTest.h>

#include <djvAV/ImageData.h>

#include <djvCore/Memory.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ImageDataTest::ImageDataTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::ImageDataTest", context)
        {}
        
        void ImageDataTest::run()
        {
            _mirror();
            _layout();
            _size();
            _info();
            _data();
            _operators();
            _serialize();
        }
        
        void ImageDataTest::_mirror()
        {
            {
                const Image::Mirror mirror;
                DJV_ASSERT(!mirror.x);
                DJV_ASSERT(!mirror.y);
            }
            
            {
                const Image::Mirror mirror(true, true);
                DJV_ASSERT(mirror.x);
                DJV_ASSERT(mirror.y);
            }
        }
        
        void ImageDataTest::_layout()
        {
            {
                const Image::Layout layout;
                DJV_ASSERT(1 == layout.alignment);
                DJV_ASSERT(Memory::getEndian() == layout.endian);
            }

            {
                const Memory::Endian endian = Memory::opposite(Memory::getEndian());
                const Image::Layout layout(
                    Image::Mirror(true, true),
                    4,
                    endian);
                DJV_ASSERT(layout.mirror.x);
                DJV_ASSERT(layout.mirror.y);
                DJV_ASSERT(4 == layout.alignment);
                DJV_ASSERT(endian == layout.endian);
            }
        }
        
        void ImageDataTest::_size()
        {
            {
                const Image::Size size;
                DJV_ASSERT(0 == size.w);
                DJV_ASSERT(0 == size.h);
            }

            {
                const Image::Size size(1, 2);
                DJV_ASSERT(1 == size.w);
                DJV_ASSERT(2 == size.h);
            }

            {
                const Image::Size size(1, 2);
                DJV_ASSERT(.5F == size.getAspectRatio());
            }
        }
        
        void ImageDataTest::_info()
        {
            {
                const Image::Info info;
                DJV_ASSERT(Image::nameDefault == info.name);
                DJV_ASSERT(1.f == info.pixelAspectRatio);
                DJV_ASSERT(Image::Type::None == info.type);
                DJV_ASSERT(!info.isValid());
            }
            
            {
                const Image::Size size(1, 2);
                const Image::Type type = Image::Type::RGB_U8;
                const Memory::Endian endian = Memory::opposite(Memory::getEndian());
                const Image::Layout layout(
                        Image::Mirror(true, true),
                        4,
                        endian);
                const Image::Info info(size, type, layout);
                DJV_ASSERT(Image::nameDefault == info.name);
                DJV_ASSERT(size == info.size);
                DJV_ASSERT(type == info.type);
                DJV_ASSERT(info.layout.mirror.x);
                DJV_ASSERT(info.layout.mirror.y);
                DJV_ASSERT(4 == info.layout.alignment);
                DJV_ASSERT(endian == info.layout.endian);
                DJV_ASSERT(info.isValid());
            }
            
            {
                const Image::Type type = Image::Type::RGB_U8;
                const Memory::Endian endian = Memory::opposite(Memory::getEndian());
                const Image::Layout layout(
                        Image::Mirror(true, true),
                        4,
                        endian);
                const Image::Info info(1, 2, type, layout);
                DJV_ASSERT(Image::nameDefault == info.name);
                DJV_ASSERT(1 == info.size.w);
                DJV_ASSERT(2 == info.size.h);
                DJV_ASSERT(type == info.type);
                DJV_ASSERT(info.layout.mirror.x);
                DJV_ASSERT(info.layout.mirror.y);
                DJV_ASSERT(4 == info.layout.alignment);
                DJV_ASSERT(endian == info.layout.endian);
                DJV_ASSERT(info.isValid());
            }
            
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                {
                    std::stringstream ss;
                    ss << "info";
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    size: " << info.size;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << info.type;
                    std::stringstream ss2;
                    ss << "    type: " << _getText(ss.str());
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    aspect ratio: " << info.getAspectRatio();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    gl format: " << info.getGLFormat();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    gl type: " << info.getGLType();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    pixel byte count: " << info.getPixelByteCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    scanline byte count: " << info.getScanlineByteCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    data byte count: " << info.getDataByteCount();
                    _print(ss.str());
                }
            }
        }
        
        void ImageDataTest::_data()
        {
            {
                auto data = Image::Data::create(Image::Info());
                DJV_ASSERT(!data->isValid());
            }
            
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                DJV_ASSERT(info == data->getInfo());
                DJV_ASSERT(info.size == data->getSize());
                DJV_ASSERT(info.size.w == data->getWidth());
                DJV_ASSERT(info.size.h == data->getHeight());
                DJV_ASSERT(info.size.getAspectRatio() == data->getAspectRatio());
                DJV_ASSERT(info.type == data->getType());
                DJV_ASSERT(info.getGLFormat() == data->getGLFormat());
                DJV_ASSERT(info.getGLType() == data->getGLType());
                DJV_ASSERT(info.layout == data->getLayout());
                DJV_ASSERT(data->isValid());
                DJV_ASSERT(info.getPixelByteCount() == data->getPixelByteCount());
                DJV_ASSERT(info.getScanlineByteCount() == data->getScanlineByteCount());
                DJV_ASSERT(info.getDataByteCount() == data->getDataByteCount());
                DJV_ASSERT(data->getData());
                DJV_ASSERT(data->getData(0));
                DJV_ASSERT(data->getData(0, 0));

                auto data2 = Image::Data::create(info);
                DJV_ASSERT(data->getUID() != data2->getUID());
            }
        }
        
        void ImageDataTest::_util()
        {
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                data->zero();
                DJV_ASSERT(0 == data->getData(0, 0));
            }
        }
        
        void ImageDataTest::_operators()
        {
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                
                const Image::Info info2(2, 1, Image::Type::RGB_U8);
                auto data2 = Image::Data::create(info2);
                DJV_ASSERT(*data != *data2);
            }
            
            {
                const Image::Info info(1, 2, Image::Type::RGB_U8);
                auto data = Image::Data::create(info);
                data->zero();
                
                auto data2 = Image::Data::create(info);
                data2->zero();
                DJV_ASSERT(*data == *data2);
                
                data2->getData()[0] = 1;
                data2->getData(0)[1] = 2;
                data2->getData(0, 1)[0] = 3;
                DJV_ASSERT(*data != *data2);
            }
            
            {
                const Image::Info info(1, 2, Image::Type::RGB_U10);
                auto data = Image::Data::create(info);
                data->zero();
                
                auto data2 = Image::Data::create(info);
                data2->zero();
                DJV_ASSERT(*data == *data2);
                
                data2->getData()[0] = 1;
                data2->getData(0)[1] = 2;
                data2->getData(0, 1)[0] = 3;
                DJV_ASSERT(*data != *data2);
            }
        }
        
        void ImageDataTest::_serialize()
        {
            {
                const Image::Size value(1, 2);
                auto json = toJSON(value);
                Image::Size value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = picojson::value(picojson::object_type, true);
                Image::Size b;
                fromJSON(json, b);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace AVTest
} // namespace djv

