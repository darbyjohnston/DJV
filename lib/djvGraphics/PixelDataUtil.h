//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#pragma once

#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/PixelData.h>

namespace djv
{
    namespace Graphics
    {
        //! \class PixelDataUtil
        //!
        //! This class provides pixel data utilities.
        class PixelDataUtil
        {
        public:
            virtual ~PixelDataUtil() = 0;

            //! Get the number of bytes in a scanline.
            static quint64 scanlineByteCount(const PixelDataInfo &);

            //! Get the number of bytes in the data.
            static quint64 dataByteCount(const PixelDataInfo &);

            //! Proxy scale pixel data.
            static void proxyScale(
                const PixelData &,
                PixelData &,
                PixelDataInfo::PROXY);

            //! Calculate the proxy scale.
            static int proxyScale(PixelDataInfo::PROXY);

            //! Calculate the size of a proxy scale.
            static glm::ivec2 proxyScale(const glm::ivec2 &, PixelDataInfo::PROXY);

            //! Calculate the size of a proxy scale.
            static Core::Box2i proxyScale(const Core::Box2i &, PixelDataInfo::PROXY);

            //! Interleave pixel data channels.
            static void planarInterleave(
                const PixelData &,
                PixelData &,
                PixelDataInfo::PROXY = PixelDataInfo::PROXY_NONE);

            //! De-interleave pixel data channels.
            static void planarDeinterleave(const PixelData &, PixelData &);

            //! Create a linear gradient.
            static void gradient(PixelData &);
        };

    } // namespace Graphics
} // namespace djv
