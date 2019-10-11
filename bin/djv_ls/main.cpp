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

#include <djvCmdLineApp/Application.h>

#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
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
            void _init(int & argc, char ** argv)
            {
                std::vector<std::string> args;
                for (int i = 0; i < argc; ++i)
                {
                    args.push_back(argv[i]);
                }
                CmdLine::Application::_init(args);

                auto io = getSystemT<AV::IO::System>();
                for (int i = 1; i < argc; ++i)
                {
                    const Core::FileSystem::FileInfo fileInfo(argv[i]);
                    switch (fileInfo.getType())
                    {
                    case Core::FileSystem::FileType::File:
                        _print(std::string(fileInfo));
                        break;
                    case Core::FileSystem::FileType::Directory:
                    {
                        std::cout << fileInfo.getPath() << ":" << std::endl;
                        Core::FileSystem::DirectoryListOptions options;
                        options.fileSequences = true;
                        options.fileSequenceExtensions = io->getSequenceExtensions();
                        for (const auto & j : Core::FileSystem::FileInfo::directoryList(fileInfo.getPath(), options))
                        {
                            _print(std::string(j));
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

            Application()
            {}

        public:
            static std::shared_ptr<Application> create(int & argc, char ** argv)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(argc, argv);
                return out;
            }

        private:
            void _print(const std::string & fileName)
            {
                std::cout << fileName << std::endl;
            }
        };

    } // namespace ls
} // namespace djv

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        ls::Application::create(argc, argv);
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}
