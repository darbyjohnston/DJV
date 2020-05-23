// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>
#include <djvCore/Vector.h>

#include <iostream>

using namespace djv;

namespace djv
{
    //! This namespace provides functionality for djv_test_pattern.
    namespace TestPattern
    {
        namespace
        {
            const size_t            frameCountDefault   = 100;
            const AV::Image::Size   sizeDefault         = AV::Image::Size(1920, 1080);
            const AV::Image::Type   typeDefault         = AV::Image::Type::RGBA_U8;

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
            Core::FileSystem::FileInfo _output;
            std::unique_ptr<size_t> _frameCount;
            std::unique_ptr<AV::Image::Size> _size;
            std::unique_ptr<AV::Image::Type> _type;
            size_t _frame = 0;
            AV::Image::Info _info;
            std::list<std::shared_ptr<AV::Image::Image> > _images;
            float _x = 0.F;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> _offscreenBuffer;
            std::shared_ptr<AV::Render2D::Render> _render;
            std::shared_ptr<AV::IO::IWrite> _write;
            std::shared_ptr<Core::Time::Timer> _statsTimer;
        };

        void Application::_init(std::list<std::string>& args)
        {
            CmdLine::Application::_init(args);

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
                _size.reset(new AV::Image::Size(sizeDefault));
            }
            if (!_type)
            {
                _type.reset(new AV::Image::Type(typeDefault));
            }
            _info = AV::Image::Info(*_size, *_type);

            _offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(_info.size, _info.type);
            _render = getSystemT<AV::Render2D::Render>();

            auto io = getSystemT<AV::IO::System>();
            AV::IO::WriteOptions writeOptions;
            AV::IO::Info ioInfo;
            ioInfo.video.push_back(_info);
            _write = io->write(_output, ioInfo, writeOptions);

            _statsTimer = Core::Time::Timer::create(shared_from_this());
            _statsTimer->setRepeating(true);
            _statsTimer->start(
                Core::Time::getTime(Core::Time::TimerValue::Slow),
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
                const AV::OpenGL::OffscreenBufferBinding binding(_offscreenBuffer);
                _render->beginFrame(*_size);
                _render->setFillColor(AV::Image::Color(.5F, .5F, .5F));
                _render->drawRect(Core::BBox2f(0.F, 0.F, static_cast<float>(_size->w), static_cast<float>(_size->h)));
                _render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));
                const float rectWidth = 10.F;
                const float rectSpacing = 100.F;
                for (float x = _x - (static_cast<int>(_x / rectSpacing) * rectSpacing) - rectWidth;
                    x < static_cast<float>(_size->w - 1 + rectWidth);
                    x = x + rectSpacing)
                {
                    _render->drawRect(Core::BBox2f(x, 0.F, rectWidth, static_cast<float>(_size->h - 1)));
                }
                _x = _x + 1.F;
                _render->endFrame();
                auto image = AV::Image::Image::create(_info);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if !defined(DJV_OPENGL_ES2)
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
                auto textSystem = getSystemT<Core::TextSystem>();
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
                        AV::Image::Size value;
                        std::stringstream ss(*i);
                        ss >> value;
                        i = args.erase(i);
                        _size.reset(new AV::Image::Size(value));
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
                        AV::Image::Type value = AV::Image::Type::None;
                        std::stringstream ss(*i);
                        ss >> value;
                        i = args.erase(i);
                        _type.reset(new AV::Image::Type(value));
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
                    _output.evalSequence();
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
            auto textSystem = getSystemT<Core::TextSystem>();
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

    } // namespace TestPattern
} // namespace djv

int main(int argc, char** argv)
{
    int r = 1;
    try
    {
        auto args = TestPattern::Application::args(argc, argv);
        auto app = TestPattern::Application::create(args);
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
