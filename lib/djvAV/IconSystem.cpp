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

#include <djvAV/IconSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Image.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            struct IconSystem::Private
            {
                int dpi = dpiDefault;
                
                FileSystem::Path getPath(const std::string & name, Context *);
            };

            void IconSystem::_init(Context * context)
            {
                ISystem::_init("djv::AV::Image::IconSystem", context);
            }

            IconSystem::IconSystem() :
                _p(new Private)
            {}

            IconSystem::~IconSystem()
            {}

            std::shared_ptr<IconSystem> IconSystem::create(Context * context)
            {
                auto out = std::shared_ptr<IconSystem>(new IconSystem);
                out->_init(context);
                return out;
            }
            
            void IconSystem::setDPI(int value)
            {
                _p->dpi = value;
            }

            std::future<IO::Info> IconSystem::getInfo(const std::string & name)
            {
                auto context = getContext();
                if (auto avSystem = context->getSystemT<AVSystem>().lock())
                {
                    if (auto thumbnailSystem = context->getSystemT<ThumbnailSystem>().lock())
                    {
                        return thumbnailSystem->getInfo(_p->getPath(name, context));
                    }
                }
                return std::future<IO::Info>();
            }

            std::future<std::shared_ptr<Image> > IconSystem::getImage(const std::string & name)
            {
                auto context = getContext();
                if (auto avSystem = context->getSystemT<AVSystem>().lock())
                {
                    if (auto thumbnailSystem = context->getSystemT<ThumbnailSystem>().lock())
                    {
                        return thumbnailSystem->getImage(_p->getPath(name, context));
                    }
                }
                return std::future<std::shared_ptr<Image> >();
            }

            FileSystem::Path IconSystem::Private::getPath(const std::string & name, Context * context)
            {
                auto out = context->getPath(FileSystem::ResourcePath::IconsDirectory);
                {
                    std::stringstream ss;
                    ss << dpi << "DPI";
                    out = FileSystem::Path(out, ss.str());
                }
                {
                    std::stringstream ss;
                    ss << name << ".png";
                    out = FileSystem::Path(out, ss.str());
                }
                return out;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
