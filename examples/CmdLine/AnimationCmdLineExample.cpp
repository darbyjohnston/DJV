//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvCmdLineApp/Application.h>

#include <djvCore/Animation.h>

#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        // Create an application.
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        auto app = CmdLine::Application::create(args);

        // Create an animation.
        auto animation = Core::Animation::Animation::create(app);
        animation->setType(Core::Animation::Type::Sine);
        animation->setRepeating(true);
        animation->start(
            0.f,
            1.f,
            std::chrono::milliseconds(1 * 1000),
            [](float value)
            {
                std::string buf;
                for (size_t i = 0; i < static_cast<size_t>(value * 40.f); ++i)
                {
                    buf += "#";
                }
                std::cout << buf << std::endl;
            },
            [app](float value)
            {
                app->exit(0);
            });

        // Run the application.
        r = app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
