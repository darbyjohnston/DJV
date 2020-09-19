// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAudioTest/AudioSystemTest.h>

#include <djvAudio/AudioSystem.h>
#include <djvAudio/AudioSystemFunc.h>

#include <djvSystem/Context.h>

#include <djvCore/StringFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::Audio;

namespace djv
{
    namespace AudioTest
    {
        AudioSystemTest::AudioSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AudioTest::AudioSystemTest", tempPath, context)
        {}
        
        void AudioSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<AudioSystem>();
                
                for (const auto& i : system->getAPIs())
                {
                    _print("API: " + i);
                }
                
                for (const auto& i : system->getDevices())
                {
                    _print("Device: " + i.name);
                    {
                        std::stringstream ss;
                        ss << "    Output channels: " << i.outputChannels;
                        _print(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << "    Input channels: " << i.inputChannels;
                        _print(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << "    Duplex channels: " << i.duplexChannels;
                        _print(ss.str());
                    }
                    {
                        std::vector<std::string> labels;
                        for (const auto& j : i.sampleRates)
                        {
                            std::stringstream ss;
                            ss << j;
                            labels.push_back(ss.str());
                        }
                        _print("    Sample rates: " + String::join(labels, ", "));
                    }
                    {
                        std::stringstream ss;
                        ss << "    Preferred sample rate: " << i.preferredSampleRate;
                        _print(ss.str());
                    }
                    {
                        std::vector<std::string> labels;
                        for (const auto& j : i.nativeFormats)
                        {
                            std::stringstream ss;
                            ss << j;
                            labels.push_back(_getText(ss.str()));
                        }
                        _print("    Native formats: " + String::join(labels, ", "));
                    }
                }
            }
        }

    } // namespace AudioTest
} // namespace djv

