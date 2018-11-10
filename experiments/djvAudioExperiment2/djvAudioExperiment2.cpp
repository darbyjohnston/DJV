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

#include <djvCore/CoreContext.h>
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
            QCoreApplication(argc, argv)
        {
            DJV_DEBUG("Application::Application");

            _context.reset(new Core::CoreContext(argc, argv));

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

            _drWavSampleData = drwav_open_and_read_file_s16(argv[1], &_drWavChannels, &_drWavSampleRate, &_drWavTotalSampleCount);
            if (!_drWavSampleData)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot open WAV file");
            }
            DJV_DEBUG_PRINT("channels = " << _drWavChannels);
            DJV_DEBUG_PRINT("sample rate = " << _drWavSampleRate);
            DJV_DEBUG_PRINT("sample count = " << static_cast<qint64>(_drWavTotalSampleCount));

            alBufferData(_alBuffer, AL_FORMAT_STEREO16, _drWavSampleData, _drWavTotalSampleCount * sizeof(drwav_int16), _drWavSampleRate);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                QString reason = "unknown";
                switch (error)
                {
                    case AL_OUT_OF_MEMORY: reason = "out of memory"; break;
                    case AL_INVALID_VALUE: reason = "invalid value"; break;
                    case AL_INVALID_ENUM: reason = "invalid enum"; break;
                    default: break;
                }
                throw Core::Error("djv::AudioExperiment2::Application", QString("Cannot set OpenAL buffer data: %1").arg(reason));
            }

            alGenSources(1, &_alSource);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot create OpenAL source");
            }

            alSourcei(_alSource, AL_BUFFER, _alBuffer);
            if ((error = alGetError()) != AL_NO_ERROR)
            {
                throw Core::Error("djv::AudioExperiment2::Application", "Cannot attach OpenAL buffer");
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
