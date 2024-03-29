// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/InfoTest.h>

#include <djvImage/Info.h>
#include <djvImage/Info.h>

#include <djvCore/Memory.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        InfoTest::InfoTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::InfoTest", tempPath, context)
        {}
        
        void InfoTest::run()
        {
            _mirror();
            _layout();
            _size();
            _info();
            _serialize();
        }
        
        void InfoTest::_mirror()
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
        
        void InfoTest::_layout()
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
        
        void InfoTest::_size()
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
        
        void InfoTest::_info()
        {
            {
                const Image::Info info;
                DJV_ASSERT(Image::defaultName == info.name);
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
                DJV_ASSERT(Image::defaultName == info.name);
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
                DJV_ASSERT(Image::defaultName == info.name);
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
                    ss << "Info";
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    Size: " << info.size;
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << info.type;
                    std::stringstream ss2;
                    ss2 << "    Type: " << _getText(ss.str());
                    _print(ss2.str());
                }
                {
                    std::stringstream ss;
                    ss << "    Aspect ratio: " << info.getAspectRatio();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    GL format: " << info.getGLFormat();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    GL type: " << info.getGLType();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    Pixel byte count: " << info.getPixelByteCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    Scanline byte count: " << info.getScanlineByteCount();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "    Data byte count: " << info.getDataByteCount();
                    _print(ss.str());
                }
            }
        }

        void InfoTest::_serialize()
        {
            {
                const Image::Size value(1, 2);
                std::stringstream ss;
                ss << value;
                Image::Size value2;
                ss >> value2;
                DJV_ASSERT(value == value2);
            }

            try
            {
                Image::Size value;
                std::stringstream ss;
                ss >> value;
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {
            }

            {
                const Image::Size value(1, 2);
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                Image::Size value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Image::Size value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {
            }
        }

    } // namespace ImageTest
} // namespace djv

