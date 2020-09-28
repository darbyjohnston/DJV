// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvRender2D/Render.h>
#include <djvRender2D/RenderSystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>

#include <djvGL/OffscreenBuffer.h>

#include <djvImage/ImageDataFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/StringFormat.h>

#include <iostream>

using namespace djv;

namespace
{
    const size_t      frameCountDefault = 100;
    const Image::Size sizeDefault       = Image::Size(1920, 1080);
    const Image::Type typeDefault       = Image::Type::RGBA_U8;

} // namespace

class Application : public CmdLine::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(std::list<std::string>&);

    Application();

public:
    static std::shared_ptr<Application> create(std::list<std::string>&);

    void run() override;
    void tick() override;

protected:
    void _parseCmdLine(std::list<std::string>&) override;
    void _printUsage() override;

private:
    std::string _output;
    std::unique_ptr<size_t> _frameCount;
    std::unique_ptr<Image::Size> _size;
    std::unique_ptr<Image::Type> _type;
    size_t _frame = 0;
    Image::Info _info;
    std::list<std::shared_ptr<Image::Image> > _images;
    float _x = 0.F;
    std::shared_ptr<GL::OffscreenBuffer> _offscreenBuffer;
    std::shared_ptr<Render2D::Render> _render;
    std::shared_ptr<AV::IO::IWrite> _write;
    std::shared_ptr<System::Timer> _statsTimer;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    Render2D::RenderSystem::create(shared_from_this());

    _parseCmdLine(args);
}

Application::Application()
{}

std::shared_ptr<Application> Application::create(std::list<std::string>& args)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(args);
    return out;
}

void Application::run()
{
    if (!_frameCount)
    {
        _frameCount.reset(new size_t(frameCountDefault));
    }
    if (!_size)
    {
        _size.reset(new Image::Size(sizeDefault));
    }
    if (!_type)
    {
        _type.reset(new Image::Type(typeDefault));
    }
    _info = Image::Info(*_size, *_type);

    _offscreenBuffer = GL::OffscreenBuffer::create(
        _info.size,
        _info.type,
        getSystemT<System::TextSystem>());
    _render = getSystemT<Render2D::Render>();

    auto io = getSystemT<AV::IO::IOSystem>();
    AV::IO::WriteOptions writeOptions;
    AV::IO::Info ioInfo;
    ioInfo.video.push_back(_info);
    const System::File::Info fileInfo(
        System::File::Path(_output),
        System::File::Type::Sequence,
        Math::Frame::Sequence(1, *_frameCount));
    _write = io->write(fileInfo, ioInfo, writeOptions);

    _statsTimer = System::Timer::create(shared_from_this());
    _statsTimer->setRepeating(true);
    _statsTimer->start(
        System::getTimerDuration(System::TimerValue::Slow),
        [this](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
        {
            std::cout << static_cast<size_t>(_frame / static_cast<float>(*_frameCount - 1) * 100.F) << "%" << std::endl;
        });

    CmdLine::Application::run();
}

void Application::tick()
{
    CmdLine::Application::tick();
    {
        std::lock_guard<std::mutex> writeLock(_write->getMutex());
        auto& writeQueue = _write->getVideoQueue();
        if (_images.size() && writeQueue.getCount() < writeQueue.getMax())
        {
            auto image = _images.front();
            _images.pop_front();
            writeQueue.addFrame(AV::IO::VideoFrame(_frame, image));
            ++_frame;
        }
        if (_frame >= *_frameCount)
        {
            writeQueue.setFinished(true);
        }
    }
    if (_frame < *_frameCount && !_images.size())
    {
        const GL::OffscreenBufferBinding binding(_offscreenBuffer);
        _render->beginFrame(*_size);
        _render->setFillColor(Image::Color(.5F, .5F, .5F));
        _render->drawRect(Math::BBox2f(0.F, 0.F, static_cast<float>(_size->w), static_cast<float>(_size->h)));
        _render->setFillColor(Image::Color(1.F, 1.F, 1.F));
        const float rectWidth = 10.F;
        const float rectSpacing = 100.F;
        for (float x = _x - (static_cast<int>(_x / rectSpacing) * rectSpacing) - rectWidth;
            x < static_cast<float>(_size->w - 1 + rectWidth);
            x = x + rectSpacing)
        {
            _render->drawRect(Math::BBox2f(x, 0.F, rectWidth, static_cast<float>(_size->h - 1)));
        }
        _x = _x + 1.F;
        _render->endFrame();
        auto image = Image::Image::create(_info);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if !defined(DJV_GL_ES2)
        glPixelStorei(GL_PACK_SWAP_BYTES, _info.layout.endian != Core::Memory::getEndian());
#endif
        glReadPixels(
            0, 0, _info.size.w, _info.size.h,
            _info.getGLFormat(),
            _info.getGLType(),
            image->getData());
        _images.push_back(image);
    }
    if (!_write->isRunning())
    {
        exit(0);
    }
}

void Application::_parseCmdLine(std::list<std::string>& args)
{
    CmdLine::Application::_parseCmdLine(args);
    if (0 == getExitCode())
    {
        auto textSystem = getSystemT<System::TextSystem>();
        auto i = args.begin();
        while (i != args.end())
        {
            if ("-frame_count" == *i)
            {
                i = args.erase(i);
                if (args.end() == i)
                {
                    throw std::runtime_error(Core::String::Format("{0}: {1}").
                        arg("-frame_count").
                        arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                }
                int value = 0;
                std::stringstream ss(*i);
                ss >> value;
                i = args.erase(i);
                _frameCount.reset(new size_t(std::max(value, 0)));
            }
            else if ("-size" == *i)
            {
                i = args.erase(i);
                if (args.end() == i)
                {
                    throw std::runtime_error(Core::String::Format("{0}: {1}").
                        arg("-size").
                        arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                }
                Image::Size value;
                std::stringstream ss(*i);
                ss >> value;
                i = args.erase(i);
                _size.reset(new Image::Size(value));
            }
            else if ("-type" == *i)
            {
                i = args.erase(i);
                if (args.end() == i)
                {
                    throw std::runtime_error(Core::String::Format("{0}: {1}").
                        arg("-type").
                        arg(textSystem->getText(DJV_TEXT("error_cannot_parse_argument"))));
                }
                Image::Type value = Image::Type::None;
                std::stringstream ss(*i);
                ss >> value;
                i = args.erase(i);
                _type.reset(new Image::Type(value));
            }
            else
            {
                ++i;
            }
        }
        if (!args.size())
        {
            _printUsage();
            exit(1);
        }
        else if (1 == args.size())
        {
            _output = args.front();
            args.pop_front();
        }
        else
        {
            throw std::runtime_error(textSystem->getText(DJV_TEXT("djv_test_pattern_output_error")));
        }
    }
}

void Application::_printUsage()
{
    auto textSystem = getSystemT<System::TextSystem>();
    std::cout << std::endl;
    std::cout << " " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_description")) << std::endl;
    std::cout << std::endl;
    std::cout << " " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_usage")) << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_output_option")) << std::endl;
    std::cout << std::endl;
    std::cout << " " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_options")) << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_option_frame_number")) << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_description_frame_number")) << frameCountDefault << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_option_resolution")) << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_description_resolution")) << sizeDefault << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_option_type")) << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_description_type")) << typeDefault << std::endl;
    std::cout << std::endl;
    std::cout << " " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_examples")) << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_test_1_dpx")) << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_render_a_test_pattern_with_the_default_values")) << std::endl;
    std::cout << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_test_1_tif_-size_3840_2160_-type_rgb_u16")) << std::endl;
    std::cout << "   " << textSystem->getText(DJV_TEXT("djv_test_pattern_cli_render_a_uhd_resolution_test_pattern_with_a_rgb_16-bit_image_type")) << std::endl;
    std::cout << std::endl;

    CmdLine::Application::_printUsage();
}

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
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
