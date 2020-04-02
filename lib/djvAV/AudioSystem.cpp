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
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

#include <rtaudio/RtAudio.h>

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
                std::vector<std::string> apis;
                std::vector<Device> devices;
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
                    p.apis.push_back(RtAudio::getApiDisplayName(i));

                    std::stringstream ss;
                    ss << "Audio API: " << RtAudio::getApiDisplayName(i);
                    _log(ss.str());
                }

                try
                {
                    p.rtAudio.reset(new RtAudio);
                    const unsigned int rtDeviceCount = p.rtAudio->getDeviceCount();
                    for (unsigned int i = 0; i < rtDeviceCount; ++i)
                    {
                        const RtAudio::DeviceInfo rtInfo = p.rtAudio->getDeviceInfo(i);
                        if (rtInfo.probed)
                        {
                            Device device;
                            device.name = rtInfo.name;
                            device.outputChannels = rtInfo.outputChannels;
                            device.inputChannels  = rtInfo.inputChannels;
                            device.duplexChannels = rtInfo.duplexChannels;
                            for (auto j : rtInfo.sampleRates)
                            {
                                device.sampleRates.push_back(j);
                            }
                            device.preferredSampleRate = rtInfo.preferredSampleRate;
                            if (rtInfo.nativeFormats & RTAUDIO_SINT8)
                            {
                                device.nativeFormats.push_back(DeviceFormat::S8);
                            }
                            if (rtInfo.nativeFormats & RTAUDIO_SINT8)
                            {
                                device.nativeFormats.push_back(DeviceFormat::S16);
                            }
                            if (rtInfo.nativeFormats & RTAUDIO_SINT16)
                            {
                                device.nativeFormats.push_back(DeviceFormat::S24);
                            }
                            if (rtInfo.nativeFormats & RTAUDIO_SINT24)
                            {
                                device.nativeFormats.push_back(DeviceFormat::S32);
                            }
                            if (rtInfo.nativeFormats & RTAUDIO_FLOAT32)
                            {
                                device.nativeFormats.push_back(DeviceFormat::F32);
                            }
                            if (rtInfo.nativeFormats & RTAUDIO_FLOAT64)
                            {
                                device.nativeFormats.push_back(DeviceFormat::F64);
                            }
                            p.devices.push_back(device);
                            {
                                std::stringstream ss;
                                ss << "Device: " << device.name;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "    Channels (output, input, duplex): " <<
                                    size_t(device.outputChannels) << ", " <<
                                    size_t(device.inputChannels) << ", " <<
                                    size_t(device.duplexChannels);
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "    Sample rates: ";
                                for (auto j : device.sampleRates)
                                {
                                    ss << j << " ";
                                }
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "    Preferred sample rate: " << device.preferredSampleRate;
                                _log(ss.str());
                            }
                            {
                                std::stringstream ss;
                                ss << "    Native formats: ";
                                auto textSystem = context->getSystemT<TextSystem>();
                                for (auto j : device.nativeFormats)
                                {
                                    std::stringstream ss2;
                                    ss2 << j;
                                    ss << textSystem->getText(ss2.str()) << " ";
                                }
                                _log(ss.str());
                            }
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    std::vector<std::string> messages;
                    auto textSystem = context->getSystemT<TextSystem>();
                    messages.push_back(textSystem->getText(DJV_TEXT("error_rtaudio_init")));
                    messages.push_back(e.what());
                    _log(String::join(messages, ' '), LogLevel::Error);
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

            const std::vector<std::string>& System::getAPIs() const
            {
                return _p->apis;
            }

            const std::vector<Device>& System::getDevices() const
            {
                return _p->devices;
            }

        } // namespace Audio
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Audio,
        DeviceFormat,
        DJV_TEXT("av_audio_device_format_s8"),
        DJV_TEXT("av_audio_device_format_s16"),
        DJV_TEXT("av_audio_device_format_s24"),
        DJV_TEXT("av_audio_device_format_s32"),
        DJV_TEXT("av_audio_device_format_f32"),
        DJV_TEXT("av_audio_device_format_f64"));

} // namespace djv

