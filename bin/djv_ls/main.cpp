// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/IOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFormat.h>

using namespace djv;

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
            System::File::Info fileInfo(args.front());
            if (fileInfo.doesExist())
            {
                _inputs.push_back(fileInfo);
            }
            else
            {
                auto textSystem = getSystemT<System::TextSystem>();
                const std::string s = Core::String::Format("{0}: {1}").
                    arg(fileInfo.getFileName()).
                    arg(textSystem->getText(DJV_TEXT("error_file_open")));
                std::cout << Core::Error::format(s) << std::endl;
            }
            args.pop_front();
        }
        if (!_inputs.size() && !hasInputs)
        {
            _inputs.push_back(System::File::Info("."));
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

    void run() override
    {
        auto io = getSystemT<AV::IO::IOSystem>();
        for (const auto& i : _inputs)
        {
            switch (i.getType())
            {
            case System::File::Type::File:
                _print(std::string(i));
                break;
            case System::File::Type::Directory:
            {
                std::cout << i.getPath() << ":" << std::endl;
                System::File::DirectoryListOptions options;
                options.sequences = true;
                options.sequenceExtensions = io->getSequenceExtensions();
                for (const auto& j : System::File::directoryList(i.getPath(), options))
                {
                    _print(j.getFileName(Math::Frame::invalid, false));
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
        auto textSystem = getSystemT<System::TextSystem>();
        std::cout << std::endl;
        std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_description")) << std::endl;
        std::cout << std::endl;
        std::cout << " " << textSystem->getText(DJV_TEXT("djv_ls_usage")) << std::endl;
        std::cout << std::endl;
        std::cout << "   " << textSystem->getText(DJV_TEXT("djv_ls_usage_format")) << std::endl;
        std::cout << std::endl;

        CmdLine::Application::_printUsage();
    }

    std::vector<System::File::Info> _inputs;

private:
    void _print(const std::string & fileName)
    {
        std::cout << fileName << std::endl;
    }
};

DJV_MAIN()
{
    int r = 1;
    try
    {
        auto args = Application::args(argc, argv);
        auto app = Application::create(args);
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
