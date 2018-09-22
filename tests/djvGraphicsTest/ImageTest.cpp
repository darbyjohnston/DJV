//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvGraphicsTest/ImageTest.h>

#include <djvGraphics/Image.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

using namespace djv::Core;
using namespace djv::Graphics;

namespace djv
{
    namespace GraphicsTest
    {
        void ImageTest::run(int &, char **)
        {
            DJV_DEBUG("ImageTest::run");
            ctors();
            operators();
        }

        void ImageTest::ctors()
        {
            DJV_DEBUG("ImageTest::ctors");
            {
                const Graphics::Image image;
                DJV_ASSERT(image.data() == 0);
            }
            {
                const Graphics::Image image(Graphics::PixelDataInfo(32, 32, Graphics::Pixel::RGBA_U8));
                Graphics::Image other(image);
                DJV_ASSERT(other.info() == image.info());
            }
        }

        void ImageTest::operators()
        {
            DJV_DEBUG("ImageTest::operators");
            {
                Graphics::Image
                    a(Graphics::PixelDataInfo(1, 1, Graphics::Pixel::L_U8)),
                    b(Graphics::PixelDataInfo(1, 1, Graphics::Pixel::L_U8));
                a.data()[0] = b.data()[0] = 127;
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != Graphics::Image());
            }
            {
                DJV_DEBUG_PRINT(Graphics::Image());
            }
        }

    } // namespace GraphicsTest
} // namespace djv
