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

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Vector.h>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_info.
    namespace info
    {
        class Application : public CmdLine::Application
        {
            DJV_NON_COPYABLE(Application);

        protected:
            void _init(std::list<std::string>& args)
            {
                CmdLine::Application::_init(args);

                _parseCmdLine(args);

                bool hasInputs = args.size();
                while (args.size())
                {
                    Core::FileSystem::FileInfo fileInfo(args.front());
                    if (fileInfo.doesExist())
                    {
                        fileInfo.evalSequence();
                        _inputs.push_back(fileInfo);
                    }
                    else
                    {
                        auto textSystem = getSystemT<Core::TextSystem>();
                        std::cout << Core::Error::format(Core::String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_open")))) << std::endl;
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
            virtual ~Application()
            {}

            static std::shared_ptr<Application> create(std::list<std::string>& args)
            {
                auto out = std::shared_ptr<Application>(new Application);
                out->_init(args);
                return out;
            }

            void run() override
            {
                auto io = getSystemT<AV::IO::System>();
                auto avSystem = getSystemT<AV::AVSystem>();
                for (const auto& i : _inputs)
                {
                    switch (i.getType())
                    {
                    case Core::FileSystem::FileType::File:
                    case Core::FileSystem::FileType::Sequence:
                        _print(i, io, avSystem);
                        break;
                    case Core::FileSystem::FileType::Directory:
                    {
                        std::cout << i.getPath() << ":" << std::endl;
                        Core::FileSystem::DirectoryListOptions options;
                        options.fileSequences = true;
                        options.fileSequenceExtensions = io->getSequenceExtensions();
                        for (const auto& j : Core::FileSystem::FileInfo::directoryList(i.getPath(), options))
                        {
                            _print(j, io, avSystem);
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

        protected:
            void _printUsage() override
            {
                auto textSystem = getSystemT<Core::TextSystem>();
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_info_description")) << std::endl;
                std::cout << std::endl;
                std::cout << " " << textSystem->getText(DJV_TEXT("djv_info_usage")) << std::endl;
                std::cout << std::endl;
                std::cout << "   " << textSystem->getText(DJV_TEXT("djv_info_usage_format")) << std::endl;
                std::cout << std::endl;

                CmdLine::Application::_printUsage();
            }

        private:
            void _print(const Core::FileSystem::FileInfo& fileInfo, std::shared_ptr<AV::IO::System>& io, std::shared_ptr<AV::AVSystem>& avSystem)
            {
                if (io->canRead(fileInfo))
                {
                    try
                    {
                        auto read = io->read(fileInfo);
                        auto info = read->getInfo().get();
                        std::cout << fileInfo << std::endl;
                        size_t i = 0;
                        for (const auto & video : info.video)
                        {
                            std::cout << "    Video track " << i << ":" << std::endl;
                            std::cout << "        Name: " << video.info.name << std::endl;
                            std::cout.precision(2);
                            std::cout << "        Size: " << video.info.size << " " << std::fixed << video.info.getAspectRatio() << std::endl;
                            std::cout << "        Type: " << video.info.type << std::endl;
                            std::cout << "        Speed: " << video.speed.toFloat() << std::endl;
                            const Core::Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                            std::cout << "        Duration: " << Core::Time::toString(video.sequence.getSize(), video.speed, timeUnits);
                            if (Core::Time::Units::Frames == timeUnits)
                            {
                                std::cout << " " << "frames";
                            }
                            std::cout << std::endl;
                            ++i;
                        }
                        i = 0;
                        for (const auto & audio : info.audio)
                        {
                            std::cout << "    Audio track " << i << ":" << std::endl;
                            std::cout << "        Channels: " << static_cast<int>(audio.info.channelCount) << std::endl;
                            std::cout << "        Type: " << audio.info.type << std::endl;
                            std::cout << "        Sample rate: " << audio.info.sampleRate << std::endl;
                            std::cout << "        Duration: " << (audio.info.sampleRate > 0 ? (audio.info.sampleCount / static_cast<float>(audio.info.sampleRate)) : 0.F) << " seconds" << std::endl;
                            ++i;
                        }
                    }
                    catch (const std::exception & e)
                    {
                        std::cout << Core::Error::format(e) << std::endl;
                    }
                }
            }

            std::vector<Core::FileSystem::FileInfo> _inputs;
        };

    } // namespace info
} // namespace djv

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = info::Application::args(argc, argv);
        auto app = info::Application::create(args);
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
