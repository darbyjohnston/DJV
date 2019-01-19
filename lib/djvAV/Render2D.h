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

#pragma once

#include <djvAV/FontSystem.h>

#include <djvCore/BBox.h>
#include <djvCore/ISystem.h>
#include <djvCore/Range.h>

#include <mutex>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;
            class Data;

        } // namespace Image

        //! This namespace provides rendering functionality.
        namespace Render
        {
            class Render2DSystem : public Core::ISystem
            {
                DJV_NON_COPYABLE(Render2DSystem);

            protected:
                void _init(Core::Context *);
                Render2DSystem();

            public:
                ~Render2DSystem();

                static std::shared_ptr<Render2DSystem> create(Core::Context *);

                void beginFrame(const glm::ivec2 &);
                void endFrame();

                void pushClipRect(const Core::BBox2f &);
                void popClipRect();

                void setFillColor(const Image::Color &);

                //! \name Primitives
                ///@{

                void drawRectangle(const Core::BBox2f &);
                void drawCircle(const glm::vec2 & pos, float radius, size_t facets = 64);

                ///@}

                //! \name Images
                ///@{

                enum class ImageType
                {
                    Static,
                    Dynamic
                };

                void drawImage(
                    const std::shared_ptr<Image::Data> &,
                    const Core::BBox2f &,
                    ImageType);

                void drawFilledImage(
                    const std::shared_ptr<Image::Data> &,
                    const Core::BBox2f &,
                    ImageType);

                ///@}

                //! \name Text
                ///@{

                void setCurrentFont(const Font::Info &);

                void drawText(
                    const std::string & text,
                    const glm::vec2 & position,
                    size_t maxLineWidth = 0);

                ///@}

            private:
                struct Private;
                std::unique_ptr<Private> _p;
            };

        } // namespace Render
    } // namespace AV
} // namespace djv

