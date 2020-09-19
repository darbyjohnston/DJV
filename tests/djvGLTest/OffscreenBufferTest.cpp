// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvGLTest/OffscreenBufferTest.h>

#include <djvGL/OffscreenBuffer.h>

#include <djvImage/ImageDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/ErrorFunc.h>

using namespace djv::Core;
using namespace djv::GL;

namespace djv
{
    namespace GLTest
    {
        OffscreenBufferTest::OffscreenBufferTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::GLTest::OffscreenBufferTest", tempPath, context)
        {}
        
        void OffscreenBufferTest::run()
        {
            _enum();
            _create();
        }

        void OffscreenBufferTest::_enum()
        {
            for (const auto& i : getOffscreenDepthTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Depth type: " + _getText(ss.str()));
            }
            
            for (const auto& i : getOffscreenSamplingEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Offscreen sampling: " + _getText(ss.str()));
            }
        }

        void OffscreenBufferTest::_create()
        {
            if (auto context = getContext().lock())
            {
                auto textSystem = context->getSystemT<System::TextSystem>();
                
                for (const auto& size : { Image::Size(64, 64), Image::Size(0, 0) })
                {
                    for (const auto type : Image::getTypeEnums())
                    {
                        try
                        {
                            {
                                std::stringstream ss;
                                ss << size;
                                _print("Size: " + ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << type;
                                _print("Type: " + _getText(ss.str()));
                            }
                            auto buffer = OffscreenBuffer::create(size, type, textSystem);
                            DJV_ASSERT(size == buffer->getSize());
                            DJV_ASSERT(type == buffer->getColorType());
                            DJV_ASSERT(buffer->getID());
                            {
                                std::stringstream ss;
                                ss << buffer->getColorID();
                                _print("Color ID: " + ss.str());
                            }
                        }
                        catch (const std::exception& e)
                        {
                            _print(Error::format(e.what()));
                        }

                        for (const auto depthType : getOffscreenDepthTypeEnums())
                        {
                            for (const auto sampling : getOffscreenSamplingEnums())
                            {
                                try
                                {
                                    {
                                        std::stringstream ss;
                                        ss << size;
                                        _print("Size: " + ss.str());
                                    }
                                    {
                                        std::stringstream ss;
                                        ss << type;
                                        _print("Type: " + _getText(ss.str()));
                                    }
                                    {
                                        std::stringstream ss;
                                        ss << depthType;
                                        _print("Depth type: " + _getText(ss.str()));
                                    }
                                    {
                                        std::stringstream ss;
                                        ss << sampling;
                                        _print("Sampling: " + _getText(ss.str()));
                                    }
                                    auto buffer = OffscreenBuffer::create(size, type, depthType, sampling, textSystem);
                                    DJV_ASSERT(size == buffer->getSize());
                                    DJV_ASSERT(type == buffer->getColorType());
                                    DJV_ASSERT(depthType == buffer->getDepthType());
                                    DJV_ASSERT(sampling == buffer->getSampling());
                                    DJV_ASSERT(buffer->getID());
                                    {
                                        std::stringstream ss;
                                        ss << buffer->getColorID();
                                        _print("Color ID: " + ss.str());
                                    }
                                    {
                                        std::stringstream ss;
                                        ss << buffer->getDepthID();
                                        _print("Depth ID: " + ss.str());
                                    }
                                }
                                catch (const std::exception& e)
                                {
                                    _print(Error::format(e.what()));
                                }
                            }
                        }
                    }
                }
            }
        }
        
    } // namespace GLTest
} // namespace djv

