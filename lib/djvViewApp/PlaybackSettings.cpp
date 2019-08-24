//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/PlaybackSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSettings::Private
        {
            std::shared_ptr<ValueSubject<bool> > startPlayback;
            std::shared_ptr<ValueSubject<bool> > playEveryFrame;
            std::shared_ptr<ValueSubject<bool> > pip;
        };

        void PlaybackSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::PlaybackSettings", context);

            DJV_PRIVATE_PTR();
            p.startPlayback = ValueSubject<bool>::create(false);
            p.playEveryFrame = ValueSubject<bool>::create(false);
            p.pip = ValueSubject<bool>::create(true);
            _load();
        }

        PlaybackSettings::PlaybackSettings() :
            _p(new Private)
        {}

        std::shared_ptr<PlaybackSettings> PlaybackSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSettings>(new PlaybackSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observeStartPlayback() const
        {
            return _p->startPlayback;
        }

        void PlaybackSettings::setStartPlayback(bool value)
        {
            _p->startPlayback->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePlayEveryFrame() const
        {
            return _p->playEveryFrame;
        }

        void PlaybackSettings::setPlayEveryFrame(bool value)
        {
            _p->playEveryFrame->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<bool> > PlaybackSettings::observePIP() const
        {
            return _p->pip;
        }

        void PlaybackSettings::setPIP(bool value)
        {
            _p->pip->setIfChanged(value);
        }

        void PlaybackSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("StartPlayback", object, p.startPlayback);
                UI::Settings::read("PlayEveryFrame", object, p.playEveryFrame);
                UI::Settings::read("PIP", object, p.pip);
            }
        }

        picojson::value PlaybackSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("StartPlayback", p.startPlayback->get(), object);
            UI::Settings::write("PlayEveryFrame", p.playEveryFrame->get(), object);
            UI::Settings::write("PIP", p.pip->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

