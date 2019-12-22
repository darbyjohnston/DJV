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

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
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
            void _init(int& argc, char** argv);

            Application();

        public:
            static std::shared_ptr<Application> create(int& argc, char** argv);

            void tick(float dt) override;

        private:
            bool _parseArgs();
            void _printUsage();

            std::string _output;
            std::unique_ptr<size_t> _frameCount;
            std::unique_ptr<AV::Image::Size> _size;
            std::unique_ptr<AV::Image::Type> _type;
            size_t _frame = 0;
            AV::Image::Info _info;
            std::list<std::shared_ptr<AV::Image::Image> > _images;
            float _x = 0.F;
            std::shared_ptr<AV::OpenGL::OffscreenBuffer> _offscreenBuffer;
            std::shared_ptr<AV::Render::Render2D> _render;
            std::shared_ptr<AV::IO::IWrite> _write;
            std::shared_ptr<Core::Time::Timer> _statsTimer;
        };

        void Application::_init(int& argc, char** argv)
        {
            std::vector<std::string> args;
            for (int i = 0; i < argc; ++i)
            {
                args.push_back(argv[i]);
            }
            CmdLine::Application::_init(args);

            if (!_parseArgs())
            {
                exit(1);
                return;
            }

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

            _offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(_info);
            _render = getSystemT<AV::Render::Render2D>();

            auto io = getSystemT<AV::IO::System>();
            Core::FileSystem::FileInfo writeFileInfo(argv[1]);
            writeFileInfo.evalSequence();
            AV::IO::WriteOptions writeOptions;
            AV::IO::Info ioInfo;
            ioInfo.video.push_back(_info);
            _write = io->write(writeFileInfo, ioInfo, writeOptions);

            _statsTimer = Core::Time::Timer::create(shared_from_this());
            _statsTimer->setRepeating(true);
            _statsTimer->start(
                Core::Time::getMilliseconds(Core::Time::TimerValue::Slow),
                [this](float)
                {
                    std::cout << static_cast<size_t>(_frame / static_cast<float>(*_frameCount - 1) * 100.F) << "%" << std::endl;
                });
        }

        Application::Application()
        {}

        std::shared_ptr<Application> Application::create(int& argc, char** argv)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }

        void Application::tick(float dt)
        {
            CmdLine::Application::tick(dt);
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
                for (float x = 0.F; x < static_cast<float>(_size->w - 1); x = x + 100.F)
                {
                    _render->drawRect(Core::BBox2f(x + _x, 0.F, 10.F, static_cast<float>(_size->h - 1)));
                }
                _x = _x + 1.F;
                if (_x >= 100.F)
                {
                    _x = 0.F;
                }
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

        bool Application::_parseArgs()
        {
            bool out = true;
            auto args = getArgs();
            auto i = args.begin();
            while (i != args.end())
            {
                if ("-h" == *i || "-help" == *i)
                {
                    out = false;
                    _printUsage();
                    break;
                }
                else if ("-frameCount" == *i)
                {
                    i = args.erase(i);
                    size_t value = 0;
                    std::stringstream ss(*i);
                    ss >> value;
                    i = args.erase(i);
                    _frameCount.reset(new size_t(value));
                }
                else if ("-size" == *i)
                {
                    i = args.erase(i);
                    AV::Image::Size value;
                    std::stringstream ss(*i);
                    ss >> value;
                    i = args.erase(i);
                    _size.reset(new AV::Image::Size(value));
                }
                else if ("-type" == *i)
                {
                    i = args.erase(i);
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
            if (2 == args.size())
            {
                _output = args[1];
            }
            else
            {
                out = false;
                _printUsage();
            }
            return out;
        }

        void Application::_printUsage()
        {
            std::cout << std::endl;
            std::cout << DJV_TEXT(" Usage:") << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   djv_test_pattern (output) [option, ...]") << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT(" Options:") << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   -frameCount (value)") << std::endl;
            std::cout << DJV_TEXT("   The number of frames to render. Default: ") << frameCountDefault << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   -size \"(width) (height)\"") << std::endl;
            std::cout << DJV_TEXT("   The image size. Default: ") << sizeDefault << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   -type (value)") << std::endl;
            std::cout << DJV_TEXT("   The image type. Default: ") << typeDefault << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT(" Examples:") << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   > djv_test_pattern test.1.dpx") << std::endl;
            std::cout << DJV_TEXT("   Render a test pattern with the default values.") << std::endl;
            std::cout << std::endl;
            std::cout << DJV_TEXT("   > djv_test_pattern test.1.tif -size '3840 2160' -type RGB_U16") << std::endl;
            std::cout << DJV_TEXT("   Render a UHD resolution test pattern with a RGB 16-bit image type.") << std::endl;
            std::cout << std::endl;
        }

    } // namespace TestPattern
} // namespace djv

int main(int argc, char** argv)
{
    int r = 0;
    try
    {
        return TestPattern::Application::create(argc, argv)->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
