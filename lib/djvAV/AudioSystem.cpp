// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/AudioSystem.h>

#include <djvAV/AudioSystemFunc.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/ErrorFunc.h>
#include <djvCore/OS.h>
#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>
#include <djvCore/TextSystem.h>

#include <RtAudio.h>

#include <sstream>

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
                                ss << "Device " << i << ": " << device.name;
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
                    {
                        std::stringstream ss;
                        ss << "Default input device: " << getDefaultInputDevice();
                        _log(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << "Default output device: " << getDefaultOutputDevice();
                        _log(ss.str());
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
                
            unsigned int System::getDefaultInputDevice()
            {
                DJV_PRIVATE_PTR();
                unsigned int out = p.rtAudio->getDefaultInputDevice();
                const unsigned int rtDeviceCount = p.rtAudio->getDeviceCount();
                std::vector<uint8_t> inputChannels;
                for (unsigned int i = 0; i < rtDeviceCount; ++i)
                {
                    const RtAudio::DeviceInfo rtInfo = p.rtAudio->getDeviceInfo(i);
                    inputChannels.push_back(rtInfo.inputChannels);
                }
                if (out < inputChannels.size())
                {
                    if (0 == inputChannels[out])
                    {
                        for (out = 0; out < rtDeviceCount; ++out)
                        {
                            if (inputChannels[out] > 0)
                            {
                                break;
                            }
                        }
                    }
                }
                return out;
            }
            
            unsigned int System::getDefaultOutputDevice()
            {
                DJV_PRIVATE_PTR();
                unsigned int out = p.rtAudio->getDefaultOutputDevice();
                const unsigned int rtDeviceCount = p.rtAudio->getDeviceCount();
                std::vector<uint8_t> outputChannels;
                for (unsigned int i = 0; i < rtDeviceCount; ++i)
                {
                    const RtAudio::DeviceInfo rtInfo = p.rtAudio->getDeviceInfo(i);
                    outputChannels.push_back(rtInfo.outputChannels);
                }
                if (out < outputChannels.size())
                {
                    if (0 == outputChannels[out])
                    {
                        for (out = 0; out < rtDeviceCount; ++out)
                        {
                            if (outputChannels[out] > 0)
                            {
                                break;
                            }
                        }
                    }
                }
                return out;
            }
        } // namespace Audio
    } // namespace AV
} // namespace djv

