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

#include <djvAV/AudioSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/Error.h>
#include <djvCore/OS.h>

#include <RtAudio.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Audio
        {
            struct System::Private
            {
                std::unique_ptr<RtAudio> rtAudio;
            };

            void System::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::Audio::System", context);
                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                {
                    std::stringstream ss;
                    ss << "RtAudio version: " << RtAudio::getVersion();
                    _log(ss.str());
                }
                std::vector<RtAudio::Api> rtAudioApis;
                RtAudio::getCompiledApi(rtAudioApis);
                for (auto i : rtAudioApis)
                {
                    std::stringstream ss;
                    ss << "RtAudio API: " << RtAudio::getApiDisplayName(i);
                    _log(ss.str());
                }

                try
                {
                    p.rtAudio.reset(new RtAudio);
                    const unsigned int deviceCount = p.rtAudio->getDeviceCount();
                    {
                        std::stringstream ss;
                        ss << "Device count: " << deviceCount;
                        _log(ss.str());
                    }
                    for (unsigned int i = 0; i < deviceCount; ++i)
                    {
                        const RtAudio::DeviceInfo info = p.rtAudio->getDeviceInfo(i);
                        if (info.probed)
                        {
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " name: " << info.name;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " output channels: " << info.outputChannels;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " inuput channels: " << info.inputChannels;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " duplex channels: " << info.duplexChannels;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " sample rates: ";
                                for (auto j : info.sampleRates)
                                {
                                    ss << j << " ";
                                }
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " preferred sample rate: " << info.preferredSampleRate;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " SINT8: " << static_cast<bool>(info.nativeFormats & RTAUDIO_SINT8);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " SINT16: " << static_cast<bool>(info.nativeFormats & RTAUDIO_SINT16);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " SINT24: " << static_cast<bool>(info.nativeFormats & RTAUDIO_SINT24);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " SINT32: " << static_cast<bool>(info.nativeFormats & RTAUDIO_SINT32);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " FLOAT32: " << static_cast<bool>(info.nativeFormats & RTAUDIO_FLOAT32);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "Device " << i << " FLOAT64: " << static_cast<bool>(info.nativeFormats & RTAUDIO_FLOAT64);
                                _log(ss.str());
                            }
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("error_rtaudio_init") << ". " << e.what();
                    _log(ss.str(), LogLevel::Error);
                }
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

        } // namespace Audio
    } // namespace AV
} // namespace djv

