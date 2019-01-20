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

#include <djvAV/AVSystem.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IconSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

using namespace gl;

#undef GL_DEBUG_SEVERITY_HIGH
#undef GL_DEBUG_SEVERITY_MEDIUM

namespace djv
{
    namespace AV
    {
        struct AVSystem::Private
        {
            std::vector<std::shared_ptr<ISystem> > systems;
        };

        void AVSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            // Create the systems.
            DJV_PRIVATE_PTR();
            p.systems.push_back(IO::System::create(context));
            p.systems.push_back(Audio::System::create(context));
            p.systems.push_back(Font::System::create(context));
            p.systems.push_back(ThumbnailSystem::create(context));
            p.systems.push_back(Image::IconSystem::create(context));
            p.systems.push_back(Render::Render2D::create(context));
        }

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
        {
            DJV_PRIVATE_PTR();
            while (p.systems.size())
            {
                auto system = p.systems.back();
                system->setParent(nullptr);
                p.systems.pop_back();
            }
        }

        std::shared_ptr<AVSystem> AVSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AVSystem>(new AVSystem);
            out->_init(context);
            return out;
        }

    } // namespace AV
} // namespace djv

