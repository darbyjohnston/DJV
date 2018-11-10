//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvAudioExperiment2.h>

#include <djvAV/AVContext.h>
#include <djvAV/IO.h>

#include <djvCore/Debug.h>
#include <djvCore/DebugLog.h>

#include <iostream>

using namespace djv;

namespace djv
{
    namespace AudioExperiment2
    {
        namespace
        {
            QStringList split(const ALCchar * value)
            {
                QStringList out;
                for (const ALCchar * p = value, * p2 = value; p2; ++p2)
                {
                    if (!*p2)
                    {
                        if (p2 > value && !*(p2 - 1))
                        {
                            break;
                        }
                        out.append(QString::fromLatin1(p, p2 - p));
                        p = p2;
                    }
                }
                return out;
            }

        } // namespace

        Application::Application(int & argc, char ** argv) :
            QGuiApplication(argc, argv)
        {
            DJV_DEBUG("Application::Application");

            _context.reset(new AV::AVContext(argc, argv));

            const ALCchar * devices = NULL;
            ALenum alEnum = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
            if (AL_TRUE == alEnum)
            {
                devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
                Q_FOREACH(auto s, split(devices))
                {
                    DJV_DEBUG_PRINT("device = " << s);
                }
            }

            _alDevice = alcOpenDevice(devices);
            if (!_alDevice)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot open OpenAL device");
            }
            _alContext = alcCreateContext(_alDevice, NULL);
            if (!_context)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot create OpenAL context");
            }
            ALCboolean r = alcMakeContextCurrent(_alContext);
            if (AL_FALSE == r)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot make OpenAL context current");
            }
            alGetError();
            alGenBuffers(1, &_alBuffer);
            ALenum error = AL_NO_ERROR;
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot create OpenAL buffer");
            }

            AV::IOInfo info;
            AV::AudioData data;
            auto load = _context->ioFactory()->load(QString(argv[1]), info);
            DJV_DEBUG_PRINT("channels = " << info.audio.channels);
            DJV_DEBUG_PRINT("sample rate = " << info.audio.sampleRate);
            DJV_DEBUG_PRINT("sample count = " << static_cast<qint64>(info.audio.sampleCount));
            load->read(data);

            alBufferData(
                _alBuffer,
                AV::Audio::toAL(info.audio.channels, info.audio.type),
                data.data(),
                data.dataByteCount(),
                info.audio.sampleRate);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application",
                    QString("Cannot set OpenAL buffer data: %1").arg(AV::Audio::alErrorString(error)));
            }

            alGenSources(1, &_alSource);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application",
                    QString("Cannot create OpenAL source").arg(AV::Audio::alErrorString(error)));
            }

            alSourcei(_alSource, AL_BUFFER, _alBuffer);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application",
                    QString("Cannot attach OpenAL buffer").arg(AV::Audio::alErrorString(error)));
            }

            alSourcePlay(_alSource);
        }

        Application::~Application()
        {
            if (_drWavSampleData)
            {
                drwav_free(_drWavSampleData);
            }
            alcMakeContextCurrent(NULL);
            if (_alContext)
            {
                alcDestroyContext(_alContext);
            }
            if (_alDevice)
            {
                alcCloseDevice(_alDevice);
            }
        }

    } // namespace ls
} // namespace djv

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        Core::CoreContext::initLibPaths(argc, argv);
        r = AudioExperiment2::Application(argc, argv).exec();
    }
    catch (const Core::Error & error)
    {
        Q_FOREACH(const Core::Error::Message & message, error.messages())
        {
            std::cout << "ERROR " <<
                message.prefix.toUtf8().data() << ": " <<
                message.string.toUtf8().data() << std::endl;
        }
    }
    catch (const std::exception & error)
    {
        std::cout << "ERROR: " << error.what() << std::endl;
    }
    return r;
}
