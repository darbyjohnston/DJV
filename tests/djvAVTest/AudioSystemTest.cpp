// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/AudioSystemTest.h>

#include <djvAV/AudioSystem.h>
#include <djvAV/AudioSystemFunc.h>

#include <djvCore/Context.h>
#include <djvCore/StringFunc.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        AudioSystemTest::AudioSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::AudioSystemTest", tempPath, context)
        {}
        
        void AudioSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<Audio::System>();
                
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

    } // namespace AVTest
} // namespace djv

