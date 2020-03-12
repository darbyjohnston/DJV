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

#include <djvCmdLineApp/Application.h>

#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Vector.h>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_ls.
    namespace ls
    {
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>& args)
            {
                CmdLine::Application::_init(args);

                bool hasInputs = args.size();
                while (args.size())
                {
                    Core::FileSystem::FileInfo fileInfo(args.front());
                    if (fileInfo.doesExist())
                    {
                        _inputs.push_back(fileInfo);
                    }
                    else
                    {
                        std::stringstream ss;
                        auto textSystem = getSystemT<Core::TextSystem>();
                        ss << textSystem->getText(DJV_TEXT("error_the_file"));
                        ss << " '" << fileInfo << "' ";
                        ss << textSystem->getText(DJV_TEXT("error_cannot_be_opened")) << ".";
                        std::cout << Core::Error::format(ss.str()) << std::endl;
                    }
                    args.pop_front();
                }
                if (!_inputs.size() && !hasInputs)
                {
                    _inputs.push_back(Core::FileSystem::FileInfo("."));
                }
            }

            Application()
            {}

        public:
            static std::shared_ptr<Application> create(std::list<std::string>& args)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(args);
                return out;
            }

            void printUsage() override
            {
                auto textSystem = getSystemT<Core::TextSystem>();
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_description")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_usage")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_ls_usage_format")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::printUsage();
            }

            void run() override
            {
                auto io = getSystemT<AV::IO::System>();
                for (const auto& i : _inputs)
                {
                    switch (i.getType())
                    {
                    case Core::FileSystem::FileType::File:
                        _print(std::string(i));
                        break;
                    case Core::FileSystem::FileType::Directory:
                    {
                        std::cout << i.getPath() << ":" << std::endl;
                        Core::FileSystem::DirectoryListOptions options;
                        options.fileSequences = true;
                        options.fileSequenceExtensions = io->getSequenceExtensions();
                        for (const auto& j : Core::FileSystem::FileInfo::directoryList(i.getPath(), options))
                        {
                            _print(j.getFileName(Core::Frame::invalid, false));
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

        private:
            void _print(const std::string & fileName)
            {
                std::cout << fileName << std::endl;
            }

            std::vector<Core::FileSystem::FileInfo> _inputs;
        };

    } // namespace ls
} // namespace djv

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = ls::Application::args(argc, argv);
        auto app = ls::Application::create(args);
        if (0 == app->getExitCode())
        {
            app->run();
        }
        r = app->getExitCode();
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}
