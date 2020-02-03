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

#include <djvAVTest/ImageConvertTest.h>

#include <djvAV/ImageConvert.h>

#include <djvCore/Context.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ImageConvertTest::ImageConvertTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::ImageConvertTest", context)
        {}
        
        void ImageConvertTest::run(const std::vector<std::string>& args)
        {
            if (auto context = getContext().lock())
            {
                const Image::Info info(64, 64, Image::Type::L_U8);
                auto data = Image::Data::create(info);
                data->getData()[0] = Image::U8Range.max;
                {
                    std::stringstream ss;
                    ss << "input: " << static_cast<uint16_t>(data->getData()[0]);
                    _print(ss.str());
                }
                
                const Image::Info info2(64, 64, Image::Type::RGBA_U8);
                auto data2 = Image::Data::create(info2);
                
                auto convert = Image::Convert::create(context->getSystemT<ResourceSystem>());
                convert->process(*data, info2, *data2);
                const Image::U8_T u8 = reinterpret_cast<const Image::U8_T*>(data2->getData())[0];
                {
                    std::stringstream ss;
                    ss << "output: " << static_cast<uint16_t>(u8);
                    _print(ss.str());
                }
                //DJV_ASSERT(Image::U8Range.max == u8);
            }
        }
                
    } // namespace AVTest
} // namespace djv

