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

#include <djvAV/AVSystem.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
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

		};

        void AVSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::AVSystem", context);

            DJV_PRIVATE_PTR();

			addDependency(context->getCoreSystem());

            auto ioSystem = IO::System::create(context);
			addDependency(ioSystem);

            auto audioSystem = Audio::System::create(context);
			addDependency(audioSystem);

            auto fontSystem = Font::System::create(context);
			addDependency(fontSystem);

            auto thumbnailSystem = ThumbnailSystem::create(context);
			thumbnailSystem->addDependency(ioSystem);
			addDependency(thumbnailSystem);
            
			auto render2D = Render::Render2D::create(context);
			render2D->addDependency(fontSystem);
			addDependency(render2D);
		}

        AVSystem::AVSystem() :
            _p(new Private)
        {}

        AVSystem::~AVSystem()
        {}

        std::shared_ptr<AVSystem> AVSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AVSystem>(new AVSystem);
            out->_init(context);
            return out;
        }

    } // namespace AV
} // namespace djv

