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
                    Settings settings;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {
                    _finish();
                }

                std::shared_ptr<Write> Write::create(
                    const std::string & fileName,
                    const Info & info,
                    const Settings& settings,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_p->settings = settings;
                    out->_init(fileName, info, resourceSystem, logSystem);
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

                void Write::_write(const std::string & fileName, const std::shared_ptr<Image::Image> & image)
                {
                }

            } // namespace TIFF
        } // namespace IO
    } // namespace AV
} // namespace djv

