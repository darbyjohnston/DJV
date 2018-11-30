//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/AudioSystem.h>

#include <djvAV/Context.h>

#include <djvCore/Error.h>

#include <iostream>

namespace djv
{
    namespace AV
    {
        namespace
        {
            std::vector<std::string> split(const ALCchar * value)
            {
                std::vector<std::string> out;
                for (const ALCchar * p = value, *p2 = value; p2; ++p2)
                {
                    if (!*p2)
                    {
                        if (p2 > value && !*(p2 - 1))
                        {
                            break;
                        }
                        out.push_back(std::string(p, p2 - p));
                        p = p2;
                    }
                }
                return out;
            }

        } // namespace

        struct AudioSystem::Private
        {
            ALCdevice * alDevice = nullptr;
            ALCcontext * alContext = nullptr;
        };

        void AudioSystem::_init(const std::shared_ptr<Context> & context)
        {
            ISystem::_init("djv::AV::AudioSystem", context);

            const ALCchar * devices = NULL;
            ALenum alEnum = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
            if (AL_TRUE == alEnum)
            {
                devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
            }

            _p->alDevice = alcOpenDevice(devices);
            if (!_p->alDevice)
            {
                throw Core::Error("djv::AV::AudioSystem", "Cannot open OpenAL device");
            }
            _p->alContext = alcCreateContext(_p->alDevice, NULL);
            if (!_p->alContext)
            {
                throw Core::Error("djv::AV::AudioSystem", "Cannot create OpenAL context");
            }
            ALCboolean r = alcMakeContextCurrent(_p->alContext);
            if (AL_FALSE == r)
            {
                throw Core::Error("djv::AV::AudioSystem", "Cannot make OpenAL context current");
            }
        }

        AudioSystem::AudioSystem() :
            _p(new Private)
        {}

        AudioSystem::~AudioSystem()
        {
            alcMakeContextCurrent(NULL);
            if (_p->alContext)
            {
                alcDestroyContext(_p->alContext);
                _p->alContext = nullptr;
            }
            if (_p->alDevice)
            {
                alcCloseDevice(_p->alDevice);
                _p->alDevice = nullptr;
            }
        }

        std::shared_ptr<AudioSystem> AudioSystem::create(const std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<AudioSystem>(new AudioSystem);
            out->_init(context);
            return out;
        }

        ALCdevice * AudioSystem::getALDevice() const
        {
            return _p->alDevice;
        }

        ALCcontext * AudioSystem::getALContext() const
        {
            return _p->alContext;
        }

    } // namespace AV
} // namespace djv

